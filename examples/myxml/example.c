void parse_event_start(struct myxml_st *myxml,
	XML_Char const *node, const XML_Char *const *atts)
{
	XML_Char const *state;
	struct events_parsing_st *self;

	self = myxml->udata;
	state = myxml_get_state(myxml);
	if (state == NULL || state == NODE_FEED)
	{
		if (myxml_node_starts_if(myxml, node, NULL, NODE_ENTRY, NULL))
		{
			g_assert(!self->wip);
			self->wip = icalcomponent_new_vevent();
			return;
		} else
			myxml_error(myxml, _("Unknown root element"));
	} else if (state == NODE_ENTRY)
	{
		if (myxml_node_is(node, atts, NODE_LINK, "rel", "edit", NULL))
		{
			parse_sequence(self, node, atts);
		} else if (myxml_node_starts_if(myxml, node, NULL,
			NODE_WHEN, NULL))
		{
			add_ical_time(self->wip,
				ICAL_DTSTART_PROPERTY, NODE_WHEN " startTime",
				myxml_get_attr(atts, "startTime"),
				self->timezone);
			add_ical_time(self->wip,
				ICAL_DTEND_PROPERTY, NODE_WHEN " endTime",
				myxml_get_attr(atts, "endTime"),
				self->timezone);
			return;
		} else if (myxml_node_starts_if(myxml, node, NULL,
			NODE_EVENTSTATUS, NULL))
		{
			parse_status(self, node, atts);
			return;
		} else if (myxml_node_starts_if(myxml, node, NULL,
			NODE_WHERE, NULL))
		{

			add_ical_string(self->wip,
				ICAL_LOCATION_PROPERTY, NODE_WHERE,
				myxml_get_attr(atts, "valueString"));
			return;
		} else if (myxml_text_node_starts_if(myxml,
			node, NULL,
			NODE_ID, NODE_PUBLISHED, NODE_UPDATED,
			NODE_TITLE, NODE_CONTENT, NULL))
		{
			return;
		} /* if */
	} else if (state == NODE_WHEN)
	{	/* Unimplemented */
		if (myxml_node_is(node, NULL, NODE_REMINDER, NULL))
		{
			printf("REMINDER: %s(%s)\n",
				myxml_get_attr(atts, "method"),
				myxml_get_attr(atts, "minuted"));
		}
	} else if (!myxml_is_ignoring(myxml))
		myxml_check_state(myxml, NODE_ID,
			NODE_EVENTSTATUS, NODE_WHERE,
			NODE_PUBLISHED, NODE_UPDATED,
			NODE_TITLE, NODE_CONTENT, NULL);

	myxml_ignored_node_started(myxml);
} /* parse_event_start */

void parse_event_end(struct myxml_st *myxml, const XML_Char *node)
{
	XML_Char const *state;
	struct events_parsing_st *self;

	self = myxml->udata;
	state = myxml_get_state(myxml);
	if (state == NODE_ID)
	{
		add_ical_string(self->wip,
			ICAL_UID_PROPERTY, NODE_ID,
			myxml_get_nice_text(myxml, NULL));
	} else if (state == NODE_PUBLISHED)
	{
		add_ical_time(self->wip,
			ICAL_CREATED_PROPERTY, NODE_PUBLISHED,
			myxml_get_nice_text(myxml, NULL),
			self->timezone);
	} else if (state == NODE_UPDATED)
	{	/* Force current time on output. */
		add_ical_time(self->wip,
			ICAL_LASTMODIFIED_PROPERTY, NODE_UPDATED,
			myxml_get_nice_text(myxml, NULL),
			self->timezone);
		if (self->iref)
		{
			myxml_add_content(myxml, gtimenow());
			goto known_node_closed;
		}
	} else if (state == NODE_TITLE)
	{
		char *gstr;

		add_ical_string(self->wip,
			ICAL_SUMMARY_PROPERTY, NODE_TITLE,
			myxml_get_nice_text(myxml, ""));
		if (self->iref && (gstr = get_ical_string(self->iref,
			TRUE, ICAL_SUMMARY_PROPERTY)) != NULL)
		{
			myxml_add_content(myxml, gstr);
			g_free(gstr);
			goto known_node_closed;
		}
	} else if (state == NODE_CONTENT)
	{
		char *gstr;

		add_ical_string(self->wip,
			ICAL_DESCRIPTION_PROPERTY, NODE_CONTENT,
			myxml_get_nice_text(myxml, ""));
		if (self->iref && (gstr = get_ical_string(self->iref,
			TRUE, ICAL_DESCRIPTION_PROPERTY)) != NULL)
		{
			myxml_add_content(myxml, gstr);
			g_free(gstr);
			goto known_node_closed;
		}
	} else if (state == NODE_ENTRY)
		g_assert(self->wip);
	else if (!myxml_is_ignoring(myxml))
		myxml_check_state(myxml,
			NODE_WHEN, NODE_EVENTSTATUS, NODE_WHERE, NULL);

	if (self->iref)
	{
		myxml_add_content(myxml, myxml_get_text(myxml, NULL));
known_node_closed:
		if (state != NODE_ENTRY)
			/* We may need to add mode items to <Entry>. */
			myxml_close_node(myxml);
	}

	myxml_node_closed(myxml);
} /* parse_event_end */

void parse_event_list_start(struct myxml_st *myxml,
	XML_Char const *node, const XML_Char *const *atts)
{
	XML_Char const *state;
	struct events_parsing_st *events;

	events = myxml->udata;
	state = myxml_get_state(myxml);
	if (state == NULL)
	{
		if (myxml_node_starts_if(myxml, node, NULL, NODE_FEED, NULL))
			return;
		myxml_error(myxml, _("Unknown root element"));
	} else if (state == NODE_FEED)
	{
		if (myxml_node_is(node, NULL, NODE_TIMEZONE, NULL))
		{
			parse_timezone(events, node, atts);
		} else if (myxml_node_is(node, NULL, NODE_ENTRY, NULL))
		{
			parse_event_start(myxml, node, atts);
			return;
		}
	} else
	{
		parse_event_start(myxml, node, atts);
		return;
	} /* if */

	myxml_ignored_node_started(myxml);
} /* parse_event_list_start */

void parse_event_list_end(struct myxml_st *myxml, const XML_Char *node)
{
	XML_Char const *state;
	struct events_parsing_st *events;

	events = myxml->udata;
	state = myxml_get_state(myxml);
	if (state != NODE_FEED)
	{
		parse_event_end(myxml, node);
		if (state == NODE_ENTRY)
		{
			events->final = g_list_prepend(events->final,
				events->wip);
			events->wip = NULL;
		}
		return;
	} /* if */

	myxml_node_closed(myxml);
} /* parse_event_list_end */

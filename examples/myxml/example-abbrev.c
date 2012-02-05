void parse_event_start(struct myxml_st *myxml,
  XML_Char const *node, const XML_Char *const *atts)
{
  XML_Char const *state = myxml_get_state(myxml);
  struct events_parsing_st *self = myxml->udata;

  if (state == NULL || state == NODE_PEOPLE) {
    if (myxml_node_starts_if(myxml, node, NULL,
                             NODE_GUY, NULL)) {
      /* ... */
      return;
    }
  } else if (state == NODE_GUY) {
    if (myxml_text_node_starts_if(myxml, node, NULL,
                                  NODE_NAME, NULL))
      return;
  }
  myxml_ignored_node_started(myxml);
}

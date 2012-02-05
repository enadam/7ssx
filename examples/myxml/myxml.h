/* Constructors */
extern gboolean myxml_new(struct myxml_st *self, struct myerr_st *myerr);
extern void myxml_free(struct myxml_st *self);

/* Parsing */
extern gboolean myxml_begin(struct myxml_st *self, void *udata,
	myxml_start_t begin_fun, myxml_close_t end_fun, ...);
extern size_t myxml_process(char const *buf, size_t sbuf, unsigned n,
	struct myxml_st *self);
extern gboolean myxml_finish(struct myxml_st *self);
extern void myxml_error(struct myxml_st *self, char const *message);

/* Examining the current state */
extern XML_Char const *myxml_get_state(struct myxml_st const *self);
extern gboolean myxml_is_ignoring(struct myxml_st const *self);
extern void myxml_check_state(struct myxml_st const *self, ...);

/* Examining a node */
extern XML_Char const *myxml_strip_namespace(XML_Char const *node);
extern XML_Char const *myxml_get_attr(
	XML_Char const *const *atts, XML_Char const *key);
extern gboolean myxml_node_is(
	XML_Char const *node, XML_Char const *const *atts,
	XML_Char const *state, ...);

/* Getting node contents */
extern void myxml_start_text(struct myxml_st *self);
extern void myxml_stop_text(struct myxml_st *self);
extern XML_Char const *myxml_get_text(struct myxml_st *self,
	XML_Char const *dflt);
extern XML_Char const *myxml_get_nice_text(struct myxml_st *self,
	XML_Char const *dflt);

/* Changing the current state */
extern void myxml_node_started(struct myxml_st *self,
	XML_Char const *state);
extern void myxml_node_closed(struct myxml_st *self);
extern void myxml_text_node_started(struct myxml_st *self,
	XML_Char const *state);
extern void myxml_ignored_node_started(struct myxml_st *self);
extern void myxml_ignored_text_node_started(struct myxml_st *self);
extern gboolean myxml_node_starts_if(struct myxml_st *self,
	XML_Char const *node, XML_Char const *const *atts,
	XML_Char const *state, ...);
extern gboolean myxml_text_node_starts_if(struct myxml_st *self,
	XML_Char const *node, XML_Char const *const *atts,
	XML_Char const *state, ...);

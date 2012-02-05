class Reader {
public: /* Constructors */
  Reader(CSink *sink);

public: /* Public methods */
  CObject *parse(CObject *cargo = NULL);

  static bool is_start_tag(CString const *tag);
  static bool is_end_tag(CString const *tag);
  static bool is_text(CString const *tag);
  static void verify_parent(CString const *parent, ...);

protected: /* Unimplemented interfaces */
  virtual CObject *event(
    CString const *tag, CObject *cargo,
    CString const *key, CObject *value) = 0;
  /* ... */
};

class CTreeBuilder:
  public gslib::gsxml::Reader
{
  /* ... */
protected: /* Implemented interfaces */
  virtual gslib::CObject *event(
    gslib::CString const *tag, gslib::CObject *cargo,
    gslib::CString const *key, gslib::CObject *value);
  /* ... */
};

public void processDocument(XmlPullParser xpp)
  throws XmlPullParserException, IOException
{
  int eventType = xpp.getEventType();
  do {
    if (eventType == xpp.START_DOCUMENT)
      System.out.println("Start document");
    else if (eventType == xpp.END_DOCUMENT)
      System.out.println("End document");
    else if (eventType == xpp.START_TAG)
      processStartElement(xpp);
    else if (eventType == xpp.END_TAG)
      processEndElement(xpp);
    else if (eventType == xpp.TEXT)
      processText(xpp);
    eventType = xpp.next();
  } while (eventType != xpp.END_DOCUMENT);
}

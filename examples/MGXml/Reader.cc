/* 
 * Scans .sin for an XML document and calls event() when an interesting
 * piece, like an element, an attribute or some text is encountered.
 * event() can be parametrized with a $cargo, which it can freely
 * change over time.  Its activation sequence goes like:
 *
 * cargo = event(tag, pargo,  Start,  patag);	// element $tag starting
 *						// in element $patag,
 *						// whose cargo is $pargo
 * cargo = event(tag, cargo,  attr1,  val1);	// an element attribute
 * cargo = event(tag, cargo,  attr2,  val2);	// another attribute
 * ...
 * cargo = event(tag, cargo,  Text,   text1);	// some text in the element
 * cargo = event(tag, cargo,  child,  value);	// child element of $tag,
 *						// $value is what $event
 *						// returned last for $child
 * cargo = event(tag, cargo,  Text,   text2);	// some more text for $tag
 * ...
 * cargo = event(tag, cargo,  Finish, pargo);	// $tag closing, time to
 *						// finalize $cargo
 *
 * Returns the cargo got from the latest invocation of the top-level element.
 * In case of error a GSLib exception is thrown and $cargo is released.
 * This means if event() throws an exception, you should not release $cargo.
 * In any case, any $value:s passed to event() are release()d by the Reader.
 *
 * Text (character data) is event()ed as a whole.  XML comments are welcome
 * and are kindly ignored.  If there is text surrounding a comment the texts
 * is event()ed separately.  Leading and trailing  whitespace is eliminated.
 *
 * This parser (hopefully) accepts all well-formed XML documents, and attepts
 * to reject most invalid ones.  An exception is that no whitespace is allowed
 * at the beginning.  This is intentional and designed with docs transmitted
 * over network in mind.  Comments are allowed everywhere except before the
 * XML declaration PIC.
 */

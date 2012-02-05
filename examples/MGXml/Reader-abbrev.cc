/*
 * FYI:
 * patag := parent's tag
 * pargo := parent's cargo
 */
cargo = event(tag, pargo, Start,  patag);

cargo = event(tag, cargo, attr1,  val1);
cargo = event(tag, cargo, attr2,  val2);
...
cargo = event(tag, cargo, Text,   text1);
cargo = event(tag, cargo, child,  value);

cargo = event(tag, cargo, Text,   text2);
...
cargo = event(tag, cargo, Finish, pargo);

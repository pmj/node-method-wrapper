var util = require('util');
var simple = require('simple');

var s = new simple.Simple("foo");

util.debug(s.numberTest(1.5, 1000));
try {
	s.numberTest(1.5, "1000"); // not a number
	util.debug("This should never be printed");
} catch (e) {
}

util.debug(s.stringTest("a", "b", "c"));
try {
	s.stringTest("a", "b"); // not enough arguments
	util.debug("This should never be printed");
} catch (e) {
}

var obj = s.objectTest("key", 5);
util.debug(obj["key"]);

util.debug(s.noArgTest());
util.debug(s.noArgTest(5)); // can have excess args

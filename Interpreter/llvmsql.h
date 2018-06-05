#pragma once

#ifndef llvmsql_h
#define llvmsql_h

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include<exception>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include<exception>
#define LL_LRLen 6

enum reserved_token_value
{
	tok_EOF = 0,
	not_mark = 1,
	minus_mark = 2,
	tilde_mark = 3,
	hat_mark = 4,
	mult_mark = 5,
	div_mark = 6,
	mod_mark = 7,
	plus_mark = 8,
	right_shift_mark = 9,
	left_shift_mark = 10,
	and_mark = 11,
	or_mark = 12,
	eq_mark = 13,
	lteqgt_mark = 14,
	gteq_mark = 15,
	gt_mark = 16,
	lteq_mark = 17,
	lt_mark = 18,
	ltgt_mark = 19,
	noteq_mark = 20,
	andand_mark = 21,
	oror_mark = 22,
	assign_mark = 23,
	double_minus_mark = 24,
	number_sign_mark = 25,
	at_mark = 26,
	quotation_mark = 27,
	apostrophe_mark = 28,
	dollar_mark = 29,
	comma_mark = 30,
	left_bracket_mark = 31,
	right_bracket_mark = 32,
	left_square_mark = 33,
	right_square_mark = 34,
	left_curly_mark = 35,
	right_curly_mark = 36,
	dot_mark = 37,
	semicolon_mark = 38,
	qusetion_mark = 39,
	escape_a = 40,
	escape_b = 41,
	escape_f = 42,
	escape_n = 43,
	escape_r = 44,
	escape_t = 45,
	escape_v = 46,
	backquote_mark = 47,
	tok_ACCESSIBLE = -1,
	tok_ACCOUNT = -2,
	tok_ACTION = -3,
	tok_ADD = -4,
	tok_AFTER = -5,
	tok_AGAINST = -6,
	tok_AGGREGATE = -7,
	tok_ALGORITHM = -8,
	tok_ALL = -9,
	tok_ALTER = -10,
	tok_ALWAYS = -11,
	tok_ANALYSE = -12,
	tok_ANALYZE = -13,
	tok_AND = -14,
	tok_ANY = -15,
	tok_AS = -16,
	tok_ASC = -17,
	tok_ASCII = -18,
	tok_ASENSITIVE = -19,
	tok_AT = -20,
	tok_AUTOEXTEND_SIZE = -21,
	tok_AUTO_INCREMENT = -22,
	tok_AVG = -23,
	tok_AVG_ROW_LENGTH = -24,
	tok_BACKUP = -25,
	tok_BDB = -26,
	tok_BEFORE = -27,
	tok_BEGIN = -28,
	tok_BERKELEYDB = -29,
	tok_BETWEEN = -30,
	tok_BIGINT = -31,
	tok_BINARY = -32,
	tok_BINLOG = -33,
	tok_BIT = -34,
	tok_BLOB = -35,
	tok_BLOCK = -36,
	tok_BOOL = -37,
	tok_BOOLEAN = -38,
	tok_BOTH = -39,
	tok_BTREE = -40,
	tok_BY = -41,
	tok_BYTE = -42,
	tok_CACHE = -43,
	tok_CALL = -44,
	tok_CASCADE = -45,
	tok_CASCADED = -46,
	tok_CASE = -47,
	tok_CATALOG_NAME = -48,
	tok_CHAIN = -49,
	tok_CHANGE = -50,
	tok_CHANGED = -51,
	tok_CHANNEL = -52,
	tok_CHAR = -53,
	tok_CHARACTER = -54,
	tok_CHARSET = -55,
	tok_CHECK = -56,
	tok_CHECKSUM = -57,
	tok_CIPHER = -58,
	tok_CLASS_ORIGIN = -59,
	tok_CLIENT = -60,
	tok_CLOSE = -61,
	tok_COALESCE = -62,
	tok_CODE = -63,
	tok_COLLATE = -64,
	tok_COLLATION = -65,
	tok_COLUMN = -66,
	tok_COLUMNS = -67,
	tok_COLUMN_FORMAT = -68,
	tok_COLUMN_NAME = -69,
	tok_COMMENT = -70,
	tok_COMMIT = -71,
	tok_COMMITTED = -72,
	tok_COMPACT = -73,
	tok_COMPLETION = -74,
	tok_COMPRESSED = -75,
	tok_COMPRESSION = -76,
	tok_CONCURRENT = -77,
	tok_CONDITION = -78,
	tok_CONNECTION = -79,
	tok_CONSISTENT = -80,
	tok_CONSTRAINT = -81,
	tok_CONSTRAINT_CATALOG = -82,
	tok_CONSTRAINT_NAME = -83,
	tok_CONSTRAINT_SCHEMA = -84,
	tok_CONTAINS = -85,
	tok_CONTEXT = -86,
	tok_CONTINUE = -87,
	tok_CONVERT = -88,
	tok_CPU = -89,
	tok_CREATE = -90,
	tok_CROSS = -91,
	tok_CUBE = -92,
	tok_CURRENT = -93,
	tok_CURRENT_DATE = -94,
	tok_CURRENT_TIME = -95,
	tok_CURRENT_TIMESTAMP = -96,
	tok_CURRENT_USER = -97,
	tok_CURSOR = -98,
	tok_CURSOR_NAME = -99,
	tok_DATA = -100,
	tok_DATABASE = -101,
	tok_DATABASES = -102,
	tok_DATAFILE = -103,
	tok_DATE = -104,
	tok_DATETIME = -105,
	tok_DAY = -106,
	tok_DAY_HOUR = -107,
	tok_DAY_MICROSECOND = -108,
	tok_DAY_MINUTE = -109,
	tok_DAY_SECOND = -110,
	tok_DEALLOCATE = -111,
	tok_DEC = -112,
	tok_DECIMAL = -113,
	tok_DECLARE = -114,
	tok_DEFAULT = -115,
	tok_DEFAULT_AUTH = -116,
	tok_DEFINER = -117,
	tok_DELAYED = -118,
	tok_DELAY_KEY_WRITE = -119,
	tok_DELETE = -120,
	tok_DESC = -121,
	tok_DESCRIBE = -122,
	tok_DES_KEY_FILE = -123,
	tok_DETERMINISTIC = -124,
	tok_DIAGNOSTICS = -125,
	tok_DIRECTORY = -126,
	tok_DISABLE = -127,
	tok_DISCARD = -128,
	tok_DISK = -129,
	tok_DISTINCT = -130,
	tok_DISTINCTROW = -131,
	tok_DIV = -132,
	tok_DO = -133,
	tok_DOUBLE = -134,
	tok_DROP = -135,
	tok_DUAL = -136,
	tok_DUMPFILE = -137,
	tok_DUPLICATE = -138,
	tok_DYNAMIC = -139,
	tok_EACH = -140,
	tok_ELSE = -141,
	tok_ELSEIF = -142,
	tok_ENABLE = -143,
	tok_ENCLOSED = -144,
	tok_ENCRYPTION = -145,
	tok_END = -146,
	tok_ENDS = -147,
	tok_ENGINE = -148,
	tok_ENGINES = -149,
	tok_ENUM = -150,
	tok_ERROR = -151,
	tok_ERRORS = -152,
	tok_ESCAPE = -153,
	tok_ESCAPED = -154,
	tok_EVENT = -155,
	tok_EVENTS = -156,
	tok_EVERY = -157,
	tok_EXCHANGE = -158,
	tok_EXECUTE = -159,
	tok_EXISTS = -160,
	tok_EXIT = -161,
	tok_EXPANSION = -162,
	tok_EXPIRE = -163,
	tok_EXPLAIN = -164,
	tok_EXPORT = -165,
	tok_EXTENDED = -166,
	tok_EXTENT_SIZE = -167,
	tok_FALSE = -168,
	tok_FAST = -169,
	tok_FAULTS = -170,
	tok_FETCH = -171,
	tok_FIELDS = -172,
	tok_FILE = -173,
	tok_FILE_BLOCK_SIZE = -174,
	tok_FILTER = -175,
	tok_FIRST = -176,
	tok_FIXED = -177,
	tok_FLOAT = -178,
	tok_FLOAT4 = -179,
	tok_FLOAT8 = -180,
	tok_FLUSH = -181,
	tok_FOLLOWS = -182,
	tok_FOR = -183,
	tok_FORCE = -184,
	tok_FOREIGN = -185,
	tok_FORMAT = -186,
	tok_FOUND = -187,
	tok_FRAC_SECOND = -188,
	tok_FROM = -189,
	tok_FULL = -190,
	tok_FULLTEXT = -191,
	tok_FUNCTION = -192,
	tok_GENERAL = -193,
	tok_GENERATED = -194,
	tok_GEOMETRY = -195,
	tok_GEOMETRYCOLLECTION = -196,
	tok_GET = -197,
	tok_GET_FORMAT = -198,
	tok_GLOBAL = -199,
	tok_GRANT = -200,
	tok_GRANTS = -201,
	tok_GROUP = -202,
	tok_GROUP_REPLICATION = -203,
	tok_HANDLER = -204,
	tok_HASH = -205,
	tok_HAVING = -206,
	tok_HELP = -207,
	tok_HIGH_PRIORITY = -208,
	tok_HOST = -209,
	tok_HOSTS = -210,
	tok_HOUR = -211,
	tok_HOUR_MICROSECOND = -212,
	tok_HOUR_MINUTE = -213,
	tok_HOUR_SECOND = -214,
	tok_IDENTIFIED = -215,
	tok_IF = -216,
	tok_IGNORE = -217,
	tok_IGNORE_SERVER_IDS = -218,
	tok_IMPORT = -219,
	tok_IN = -220,
	tok_INDEX = -221,
	tok_INDEXES = -222,
	tok_INFILE = -223,
	tok_INITIAL_SIZE = -224,
	tok_INNER = -225,
	tok_INNODB = -226,
	tok_INOUT = -227,
	tok_INSENSITIVE = -228,
	tok_INSERT = -229,
	tok_INSERT_METHOD = -230,
	tok_INSTALL = -231,
	tok_INSTANCE = -232,
	tok_INT = -233,
	tok_INT1 = -234,
	tok_INT2 = -235,
	tok_INT3 = -236,
	tok_INT4 = -237,
	tok_INT8 = -238,
	tok_INTEGER = -239,
	tok_INTERVAL = -240,
	tok_INTO = -241,
	tok_INVOKER = -242,
	tok_IO = -243,
	tok_IO_AFTER_GTIDS = -244,
	tok_IO_BEFORE_GTIDS = -245,
	tok_IO_THREAD = -246,
	tok_IPC = -247,
	tok_IS = -248,
	tok_ISOLATION = -249,
	tok_ISSUER = -250,
	tok_ITERATE = -251,
	tok_JOIN = -252,
	tok_JSON = -253,
	tok_KEY = -254,
	tok_KEYS = -255,
	tok_KEY_BLOCK_SIZE = -256,
	tok_KILL = -257,
	tok_LANGUAGE = -258,
	tok_LAST = -259,
	tok_LEADING = -260,
	tok_LEAVE = -261,
	tok_LEAVES = -262,
	tok_LEFT = -263,
	tok_LESS = -264,
	tok_LEVEL = -265,
	tok_LIKE = -266,
	tok_LIMIT = -267,
	tok_LINEAR = -268,
	tok_LINES = -269,
	tok_LINESTRING = -270,
	tok_LIST = -271,
	tok_LOAD = -272,
	tok_LOCAL = -273,
	tok_LOCALTIME = -274,
	tok_LOCALTIMESTAMP = -275,
	tok_LOCK = -276,
	tok_LOCKS = -277,
	tok_LOGFILE = -278,
	tok_LOGS = -279,
	tok_LONG = -280,
	tok_LONGBLOB = -281,
	tok_LONGTEXT = -282,
	tok_LOOP = -283,
	tok_LOW_PRIORITY = -284,
	tok_MASTER = -285,
	tok_MASTER_AUTO_POSITION = -286,
	tok_MASTER_BIND = -287,
	tok_MASTER_CONNECT_RETRY = -288,
	tok_MASTER_DELAY = -289,
	tok_MASTER_HEARTBEAT_PERIOD = -290,
	tok_MASTER_HOST = -291,
	tok_MASTER_LOG_FILE = -292,
	tok_MASTER_LOG_POS = -293,
	tok_MASTER_PASSWORD = -294,
	tok_MASTER_PORT = -295,
	tok_MASTER_RETRY_COUNT = -296,
	tok_MASTER_SERVER_ID = -297,
	tok_MASTER_SSL = -298,
	tok_MASTER_SSL_CA = -299,
	tok_MASTER_SSL_CAPATH = -300,
	tok_MASTER_SSL_CERT = -301,
	tok_MASTER_SSL_CIPHER = -302,
	tok_MASTER_SSL_CRL = -303,
	tok_MASTER_SSL_CRLPATH = -304,
	tok_MASTER_SSL_KEY = -305,
	tok_MASTER_SSL_VERIFY_SERVER_CERT = -306,
	tok_MASTER_TLS_VERSION = -307,
	tok_MASTER_USER = -308,
	tok_MATCH = -309,
	tok_MAXVALUE = -310,
	tok_MAX_CONNECTIONS_PER_HOUR = -311,
	tok_MAX_QUERIES_PER_HOUR = -312,
	tok_MAX_ROWS = -313,
	tok_MAX_SIZE = -314,
	tok_MAX_STATEMENT_TIME = -315,
	tok_MAX_UPDATES_PER_HOUR = -316,
	tok_MAX_USER_CONNECTIONS = -317,
	tok_MEDIUM = -318,
	tok_MEDIUMBLOB = -319,
	tok_MEDIUMINT = -320,
	tok_MEDIUMTEXT = -321,
	tok_MEMORY = -322,
	tok_MERGE = -323,
	tok_MESSAGE_TEXT = -324,
	tok_MICROSECOND = -325,
	tok_MIDDLEINT = -326,
	tok_MIGRATE = -327,
	tok_MINUTE = -328,
	tok_MINUTE_MICROSECOND = -329,
	tok_MINUTE_SECOND = -330,
	tok_MIN_ROWS = -331,
	tok_MOD = -332,
	tok_MODE = -333,
	tok_MODIFIES = -334,
	tok_MODIFY = -335,
	tok_MONTH = -336,
	tok_MULTILINESTRING = -337,
	tok_MULTIPOINT = -338,
	tok_MULTIPOLYGON = -339,
	tok_MUTEX = -340,
	tok_MYSQL_ERRNO = -341,
	tok_NAME = -342,
	tok_NAMES = -343,
	tok_NATIONAL = -344,
	tok_NATURAL = -345,
	tok_NCHAR = -346,
	tok_NDB = -347,
	tok_NDBCLUSTER = -348,
	tok_NEVER = -349,
	tok_NEW = -350,
	tok_NEXT = -351,
	tok_NO = -352,
	tok_NODEGROUP = -353,
	tok_NONBLOCKING = -354,
	tok_NONE = -355,
	tok_NOT = -356,
	tok_NO_WAIT = -357,
	tok_NO_WRITE_TO_BINLOG = -358,
	tok_NULL = -359,
	tok_NUMBER = -360,
	tok_NUMERIC = -361,
	tok_NVARCHAR = -362,
	tok_OFFSET = -363,
	tok_OLD_PASSWORD = -364,
	tok_ON = -365,
	tok_ONE = -366,
	tok_ONLY = -367,
	tok_OPEN = -368,
	tok_OPTIMIZE = -369,
	tok_OPTIMIZER_COSTS = -370,
	tok_OPTION = -371,
	tok_OPTIONALLY = -372,
	tok_OPTIONS = -373,
	tok_OR = -374,
	tok_ORDER = -375,
	tok_OUT = -376,
	tok_OUTER = -377,
	tok_OUTFILE = -378,
	tok_OWNER = -379,
	tok_PACK_KEYS = -380,
	tok_PAGE = -381,
	tok_PARSER = -382,
	tok_PARSE_GCOL_EXPR = -383,
	tok_PARTIAL = -384,
	tok_PARTITION = -385,
	tok_PARTITIONING = -386,
	tok_PARTITIONS = -387,
	tok_PASSWORD = -388,
	tok_PHASE = -389,
	tok_PLUGIN = -390,
	tok_PLUGINS = -391,
	tok_PLUGIN_DIR = -392,
	tok_POINT = -393,
	tok_POLYGON = -394,
	tok_PORT = -395,
	tok_PRECEDES = -396,
	tok_PRECISION = -397,
	tok_PREPARE = -398,
	tok_PRESERVE = -399,
	tok_PREV = -400,
	tok_PRIMARY = -401,
	tok_PRIVILEGES = -402,
	tok_PROCEDURE = -403,
	tok_PROCESSLIST = -404,
	tok_PROFILE = -405,
	tok_PROFILES = -406,
	tok_PROXY = -407,
	tok_PURGE = -408,
	tok_QUARTER = -409,
	tok_QUERY = -410,
	tok_QUICK = -411,
	tok_RANGE = -412,
	tok_READ = -413,
	tok_READS = -414,
	tok_READ_ONLY = -415,
	tok_READ_WRITE = -416,
	tok_REAL = -417,
	tok_REBUILD = -418,
	tok_RECOVER = -419,
	tok_REDOFILE = -420,
	tok_REDO_BUFFER_SIZE = -421,
	tok_REDUNDANT = -422,
	tok_REFERENCES = -423,
	tok_REGEXP = -424,
	tok_RELAY = -425,
	tok_RELAYLOG = -426,
	tok_RELAY_LOG_FILE = -427,
	tok_RELAY_LOG_POS = -428,
	tok_RELAY_THREAD = -429,
	tok_RELEASE = -430,
	tok_RELOAD = -431,
	tok_REMOVE = -432,
	tok_RENAME = -433,
	tok_REORGANIZE = -434,
	tok_REPAIR = -435,
	tok_REPEAT = -436,
	tok_REPEATABLE = -437,
	tok_REPLACE = -438,
	tok_REPLICATE_DO_DB = -439,
	tok_REPLICATE_DO_TABLE = -440,
	tok_REPLICATE_IGNORE_DB = -441,
	tok_REPLICATE_IGNORE_TABLE = -442,
	tok_REPLICATE_REWRITE_DB = -443,
	tok_REPLICATE_WILD_DO_TABLE = -444,
	tok_REPLICATE_WILD_IGNORE_TABLE = -445,
	tok_REPLICATION = -446,
	tok_REQUIRE = -447,
	tok_RESET = -448,
	tok_RESIGNAL = -449,
	tok_RESTORE = -450,
	tok_RESTRICT = -451,
	tok_RESUME = -452,
	tok_RETURN = -453,
	tok_RETURNED_SQLSTATE = -454,
	tok_RETURNS = -455,
	tok_REVERSE = -456,
	tok_REVOKE = -457,
	tok_RIGHT = -458,
	tok_RLIKE = -459,
	tok_ROLLBACK = -460,
	tok_ROLLUP = -461,
	tok_ROTATE = -462,
	tok_ROUTINE = -463,
	tok_ROW = -464,
	tok_ROWS = -465,
	tok_ROW_COUNT = -466,
	tok_ROW_FORMAT = -467,
	tok_RTREE = -468,
	tok_SAVEPOINT = -469,
	tok_SCHEDULE = -470,
	tok_SCHEMA = -471,
	tok_SCHEMAS = -472,
	tok_SCHEMA_NAME = -473,
	tok_SECOND = -474,
	tok_SECOND_MICROSECOND = -475,
	tok_SECURITY = -476,
	tok_SELECT = -477,
	tok_SENSITIVE = -478,
	tok_SEPARATOR = -479,
	tok_SERIAL = -480,
	tok_SERIALIZABLE = -481,
	tok_SERVER = -482,
	tok_SESSION = -483,
	tok_SET = -484,
	tok_SHARE = -485,
	tok_SHOW = -486,
	tok_SHUTDOWN = -487,
	tok_SIGNAL = -488,
	tok_SIGNED = -489,
	tok_SIMPLE = -490,
	tok_SLAVE = -491,
	tok_SLOW = -492,
	tok_SMALLINT = -493,
	tok_SNAPSHOT = -494,
	tok_SOCKET = -495,
	tok_SOME = -496,
	tok_SONAME = -497,
	tok_SOUNDS = -498,
	tok_SOURCE = -499,
	tok_SPATIAL = -500,
	tok_SPECIFIC = -501,
	tok_SQL = -502,
	tok_SQLEXCEPTION = -503,
	tok_SQLSTATE = -504,
	tok_SQLWARNING = -505,
	tok_SQL_AFTER_GTIDS = -506,
	tok_SQL_AFTER_MTS_GAPS = -507,
	tok_SQL_BEFORE_GTIDS = -508,
	tok_SQL_BIG_RESULT = -509,
	tok_SQL_BUFFER_RESULT = -510,
	tok_SQL_CACHE = -511,
	tok_SQL_CALC_FOUND_ROWS = -512,
	tok_SQL_NO_CACHE = -513,
	tok_SQL_SMALL_RESULT = -514,
	tok_SQL_THREAD = -515,
	tok_SQL_TSI_DAY = -516,
	tok_SQL_TSI_FRAC_SECOND = -517,
	tok_SQL_TSI_HOUR = -518,
	tok_SQL_TSI_MINUTE = -519,
	tok_SQL_TSI_MONTH = -520,
	tok_SQL_TSI_QUARTER = -521,
	tok_SQL_TSI_SECOND = -522,
	tok_SQL_TSI_WEEK = -523,
	tok_SQL_TSI_YEAR = -524,
	tok_SSL = -525,
	tok_STACKED = -526,
	tok_START = -527,
	tok_STARTING = -528,
	tok_STARTS = -529,
	tok_STATS_AUTO_RECALC = -530,
	tok_STATS_PERSISTENT = -531,
	tok_STATS_SAMPLE_PAGES = -532,
	tok_STATUS = -533,
	tok_STOP = -534,
	tok_STORAGE = -535,
	tok_STORED = -536,
	tok_STRAIGHT_JOIN = -537,
	tok_STRING = -538,
	tok_STRIPED = -539,
	tok_SUBCLASS_ORIGIN = -540,
	tok_SUBJECT = -541,
	tok_SUBPARTITION = -542,
	tok_SUBPARTITIONS = -543,
	tok_SUPER = -544,
	tok_SUSPEND = -545,
	tok_SWAPS = -546,
	tok_SWITCHES = -547,
	tok_TABLE = -548,
	tok_TABLES = -549,
	tok_TABLESPACE = -550,
	tok_TABLE_CHECKSUM = -551,
	tok_TABLE_NAME = -552,
	tok_TEMPORARY = -553,
	tok_TEMPTABLE = -554,
	tok_TERMINATED = -555,
	tok_TEXT = -556,
	tok_THAN = -557,
	tok_THEN = -558,
	tok_TIME = -559,
	tok_TIMESTAMP = -560,
	tok_TIMESTAMPADD = -561,
	tok_TIMESTAMPDIFF = -562,
	tok_TINYBLOB = -563,
	tok_TINYINT = -564,
	tok_TINYTEXT = -565,
	tok_TO = -566,
	tok_TRAILING = -567,
	tok_TRANSACTION = -568,
	tok_TRIGGER = -569,
	tok_TRIGGERS = -570,
	tok_TRUE = -571,
	tok_TRUNCATE = -572,
	tok_TYPE = -573,
	tok_TYPES = -574,
	tok_UNCOMMITTED = -575,
	tok_UNDEFINED = -576,
	tok_UNDO = -577,
	tok_UNDOFILE = -578,
	tok_UNDO_BUFFER_SIZE = -579,
	tok_UNICODE = -580,
	tok_UNINSTALL = -581,
	tok_UNION = -582,
	tok_UNIQUE = -583,
	tok_UNKNOWN = -584,
	tok_UNLOCK = -585,
	tok_UNSIGNED = -586,
	tok_UNTIL = -587,
	tok_UPDATE = -588,
	tok_UPGRADE = -589,
	tok_USAGE = -590,
	tok_USE = -591,
	tok_USER = -592,
	tok_USER_RESOURCES = -593,
	tok_USE_FRM = -594,
	tok_USING = -595,
	tok_UTC_DATE = -596,
	tok_UTC_TIME = -597,
	tok_UTC_TIMESTAMP = -598,
	tok_VALIDATION = -599,
	tok_VALUE = -600,
	tok_VALUES = -601,
	tok_VARBINARY = -602,
	tok_VARCHAR = -603,
	tok_VARCHARACTER = -604,
	tok_VARIABLES = -605,
	tok_VARYING = -606,
	tok_VIEW = -607,
	tok_VIRTUAL = -608,
	tok_WAIT = -609,
	tok_WARNINGS = -610,
	tok_WEEK = -611,
	tok_WEIGHT_STRING = -612,
	tok_WHEN = -613,
	tok_WHERE = -614,
	tok_WHILE = -615,
	tok_WITH = -616,
	tok_WITHOUT = -617,
	tok_WORK = -618,
	tok_WRAPPER = -619,
	tok_WRITE = -620,
	tok_X509 = -621,
	tok_XA = -622,
	tok_XID = -623,
	tok_XML = -624,
	tok_XOR = -625,
	tok_YEAR = -626,
	tok_YEAR_MONTH = -627,
	tok_ZEROFILL = -628

};

enum status
{
	blank,
	comment,
	literal_string,
	literal_int,
	literal_double,
	id,
	symbol,
	eof
};

struct val;
class value;

class int_value;

class double_value;

class string_value;

class reserved_value;

class id_value;

class eof_value;

class token;

class value
{
	friend val;
public:
	virtual ~value() = default;
};

class blank_value :public value
{
	friend val;
	char ch = ' ';
public:
	blank_value(char ch):ch(ch){}
};

class int_value :public value
{
	friend val;
	int i;
public:
	int_value(int i) :i(i) {}
};

class double_value :public value
{
	friend val;
	double d;
public:
	double_value(double d) :d(d) {}
};

class string_value :public value
{
	friend val;
	std::string s;
public:
	string_value(std::string s) :s(s) {}
};

class reserved_value :public value
{
	friend val;
	int r;
public:
	reserved_value(int r) :r(r) {}
};

class id_value :public value
{
	friend val;
	std::string id;
public:
	id_value(std::string id) :id(id) {}
};

class eof_value :public value
{
	const int v = 0;
};

struct val
{
	std::string string_literal; // ' ""
	int int_literal = 0;    // 先判断是否是 int
	double double_literal = 0.; // 后判断是否是 double
	int symbol_mark = 0;
	std::string IdentifierStr;
	char ch = ' ';
	val() = default;
	val(eof_value ev):ch(EOF){}
	val& operator=(const string_value& s)
	{
		string_literal = s.s;
		return *this;
	}
	val& operator=(const int_value& i)
	{
		int_literal = i.i;
		return *this;
	}
	val& operator=(const double_value& d)
	{
		double_literal = d.d;
		return *this;
	}
	val& operator=(const reserved_value& r)
	{
		symbol_mark = r.r;
		return *this;
	}
	val& operator=(const id_value& i)
	{
		IdentifierStr = i.id;
		return *this;
	}
	val& operator=(const blank_value& blank)
	{
		ch = blank.ch;
		return *this;
	}
};

class token
{
public:
	int token_kind;
	val token_value;

	token() = default;
	token(const token& a) :token_kind(a.token_kind), token_value(a.token_value) {}
	token& operator=(const token& b)
	{
		token_kind = b.token_kind;
		token_value = b.token_value;
		return *this;
	}
	
};

token gettok();





class table_col :public val
{
	using pair = std::pair<std::string, std::string>;
	pair p;
public:
	typename pair::first_type& table_name = p.first;
	typename pair::second_type& col_name = p.second;
	table_col() = default;
	table_col(const std::string& table_name, const std::string& col_name) :p(table_name, col_name) {}
};




class ExprAST
{
public:
	std::unique_ptr<ExpAST> lhs = nullptr;
	int op;
	std::unique_ptr<ExprAST> rhs = nullptr;
	virtual ~ExprAST() = default;
	ExprAST() = default;
	ExprAST(std::unique_ptr<ExpAST> lhs):
		lhs(std::move(lhs)){}
	ExprAST(std::unique_ptr<ExpAST> lhs, int op, std::unique_ptr<ExprAST> rhs):
		lhs(std::move(lhs)),op(op) ,rhs(std::move(rhs)){}
};

class ExpAST :public ExprAST
{
public:
	std::unique_ptr<ExprAST> expr = nullptr;
	std::unique_ptr<BooleanPrimaryAST> bp = nullptr;
	ExpAST() = default;
	ExpAST(std::unique_ptr<ExprAST> expr) :
		expr(std::move(expr)){}
	ExpAST(std::unique_ptr<BooleanPrimaryAST> bp):
		bp(std::move(bp)){}
};

class BooleanPrimaryAST :public ExpAST
{
public:
	std::unique_ptr<BooleanPrimaryAST> bp = nullptr;
	std::unique_ptr<PredicateAST> p = nullptr;
	bool flag;	// true for all, false for any
	int op;
	std::unique_ptr<SubqueryAST> sub = nullptr;
	BooleanPrimaryAST() = default;
	BooleanPrimaryAST(std::unique_ptr<BooleanPrimaryAST> bp, bool flag) :
		bp(std::move(bp)), flag(flag){}
	BooleanPrimaryAST(std::unique_ptr<PredicateAST> p):
		p(std::move(p)){}
	BooleanPrimaryAST(std::unique_ptr<BooleanPrimaryAST>bp , std::unique_ptr<PredicateAST> p, int op):
		bp(std::move(bp)),p(std::move(p)), op(op){}
	BooleanPrimaryAST(std::unique_ptr<BooleanPrimaryAST>bp, std::unique_ptr<SubqueryAST>sub,int op):
		bp(std::move(bp)),sub(std::move(sub)),op(op){}
};

class PredicateAST :public BooleanPrimaryAST
{
public:
	std::unique_ptr<BitExprAST> bitexpr = nullptr;
	bool flag;
	std::unique_ptr<BitExprAST> rhs = nullptr;
	std::unique_ptr<SubqueryAST> sub = nullptr;
	std::vector<std::unique_ptr<ExprAST>> exprs;
	PredicateAST() = default;
	PredicateAST(std::unique_ptr<BitExprAST> bitexpr):
		bitexpr(std::move(bitexpr)){}
	PredicateAST(std::unique_ptr<BitExprAST> bitexpr, std::unique_ptr<SubqueryAST> sub, bool flag) :
		bitexpr(std::move(bitexpr)),sub(std::move(sub)),flag(flag){}
	PredicateAST(std::unique_ptr<BitExprAST> bitexpr, std::unique_ptr<BitExprAST> rhs, bool flag) :
		bitexpr(std::move(bitexpr)), rhs(std::move(rhs)), flag(flag) {}
	PredicateAST(std::unique_ptr<BitExprAST> bitexpr, std::vector<std::unique_ptr<ExprAST>> exprs, bool flag) :
			bitexpr(std::move(bitexpr)),exprs(std::move(exprs)), flag(flag) {}
};

class BitExprAST :public PredicateAST
{
public:
	std::unique_ptr<BitExpAST> bitexp = nullptr;
	int op;
	std::unique_ptr<BitExprAST> bitexpr = nullptr;
	BitExprAST() = default;
	BitExprAST(std::unique_ptr<BitExpAST> bitexp):
		bitexp(std::move(bitexp)){}
	BitExprAST(std::unique_ptr<BitExprAST> bitexpr,int op,std::unique_ptr<BitExpAST> bitexp) :
		bitexp(std::move(bitexp)),op(op),bitexpr(std::move(bitexpr)){}
};

class BitExpAST :public BitExprAST
{
public:
	std::unique_ptr<BitExpAST> bitexp = nullptr;
	int op;
	std::unique_ptr<BitExAST> bitex = nullptr;
	BitExpAST() = default;
	BitExpAST(std::unique_ptr<BitExAST> bitex) :
		bitex(std::move(bitex)) {}
	BitExpAST(std::unique_ptr<BitExAST> bitex,int op, std::unique_ptr<BitExpAST> bitexp) :
		bitexp(std::move(bitexp)), op(op), bitex(std::move(bitex)) {}
};

class BitExAST :public BitExpAST
{
public:
	int mark;
	std::unique_ptr<BitExAST> bitex = nullptr;
	std::unique_ptr<SimpleExprAST> SE = nullptr;
	BitExAST() = default;
	BitExAST(int mark, std::unique_ptr<BitExAST> bitex) :
		mark(mark), bitex(std::move(bitex)) {}
	BitExAST(std::unique_ptr<SimpleExprAST> SE) :
		SE(std::move(SE)) {}
};

class SimpleExprAST :public BitExAST
{
public:
	std::unique_ptr<IdAST> id = nullptr;
	std::unique_ptr<CallAST> call = nullptr;
	std::unique_ptr<TablecolAST> tablecol = nullptr;
	std::unique_ptr<ExprAST> expr = nullptr;
	std::unique_ptr<SubqueryAST> sub = nullptr;
	std::unique_ptr<ExistsSubqueryAST> exists = nullptr;
	std::unique_ptr<LiteralAST> lit = nullptr;

	SimpleExprAST() = default;
	SimpleExprAST(std::unique_ptr<IdAST> id):id(std::move(id)){}
	SimpleExprAST(std::unique_ptr<CallAST> call) :call(std::move(call)) {}
	SimpleExprAST(std::unique_ptr<TablecolAST> tablecol) :tablecol(std::move(tablecol)) {}
	SimpleExprAST(std::unique_ptr<ExprAST> expr) :expr(std::move(expr)) {}
	SimpleExprAST(std::unique_ptr<SubqueryAST> sub) :sub(std::move(sub)) {}
	SimpleExprAST(std::unique_ptr<ExistsSubqueryAST> exists) :exists(std::move(exists)) {}
	SimpleExprAST(std::unique_ptr<LiteralAST> lit) :lit(std::move(lit)) {}
};

class IdAST final :public SimpleExprAST
{
public:
	std::unique_ptr<std::string> id=nullptr;
	IdAST(std::unique_ptr<std::string> id) :id(std::move(id)) {}
};

class ColdefAST
{
public:
	std::unique_ptr<std::string> col_name = nullptr;
	int dtype = 0;
	bool nullable = true;
	bool unique = false;
	bool primary = false;
	int n ;
	ColdefAST() = default;
	ColdefAST(std::unique_ptr<std::string> colname, int dtype, bool nullable, bool unique, bool primary) :
		col_name(std::move(col_name)), dtype(dtype), nullable(nullable), unique(unique), primary(primary) {}
	ColdefAST(std::unique_ptr<std::string> colname, int dtype, bool nullable, bool unique, bool primary, int n) :
		col_name(std::move(col_name)), dtype(dtype), nullable(nullable), unique(unique), primary(primary), n(n) {}
};

class TablecolAST final :SimpleExprAST
{
public:
	std::unique_ptr<std::string> table_name = nullptr;
	std::unique_ptr<std::string> col_name = nullptr;
	TablecolAST(std::unique_ptr<std::string> col_name): col_name(std::move(col_name) ) {}
	TablecolAST(std::unique_ptr<std::string> table_name, std::unique_ptr<std::string> col_name):
		table_name(std::move(table_name)), col_name(std::move(col_name)) {}
};

class CallAST final :public SimpleExprAST
{
public:
	std::unique_ptr<std::string> callee;
	std::vector<std::unique_ptr<ExprAST>> args;
	CallAST(std::unique_ptr<std::string> callee, std::vector<std::unique_ptr<ExprAST>> args)
		: callee(std::move(callee) ), args(std::move(args)) {}
};

class LiteralAST :public SimpleExprAST
{
public:
	std::unique_ptr<IntLiteralAST> intvalue = nullptr;
	std::unique_ptr<DoubleLiteralAST> doublevalue = nullptr;
	std::unique_ptr<StringLiteralAST> stringvalue = nullptr;
	LiteralAST(std::unique_ptr<IntLiteralAST> intvalue):intvalue(std::move(intvalue)){}
	LiteralAST(std::unique_ptr<DoubleLiteralAST> doublevalue) :doublevalue(std::move(doublevalue)) {}
	LiteralAST(std::unique_ptr<StringLiteralAST> stringvalue) :stringvalue(std::move(stringvalue)) {}
};

class IntLiteralAST final :public LiteralAST
{
public:
	std::unique_ptr<int> value = nullptr;
	IntLiteralAST(std::unique_ptr<int> value) :value(std::move(value)) {}
};

class DoubleLiteralAST final : public LiteralAST
{
public:
	std::unique_ptr<double> value = nullptr;
	DoubleLiteralAST(std::unique_ptr<double> value) :value(std::move(value)) {}
};

class StringLiteralAST final :public LiteralAST
{
public:
	std::unique_ptr<std::string> value = nullptr;
	StringLiteralAST(std::unique_ptr<std::string> value) :value(std::move(value)) {}
	
};

class ParenExprAST final :public SimpleExprAST
{
public:
	std::unique_ptr<ExprAST> expr;
	ParenExprAST(std::unique_ptr<ExprAST> expr) :
		expr(std::move(expr)) {}
};









class SubqueryAST;

class TableRefAST
{
public:
	std::string topalias;
	TableRefAST() = default;
	TableRefAST(const std::string& topalias) :topalias(topalias) {}
};

class TableRefsAST
{
	std::vector<std::unique_ptr<TableRefAST>> refs;
public:
	TableRefsAST(std::vector<std::unique_ptr<TableRefAST>> refs) :refs(std::move(refs)) {}
};

class JoinCondAST
{
	;
};



class TableFactorAST :public TableRefAST
{
	;
};

class JoinTableAST :public TableRefAST
{
	;
};

class TableNameAST final :public TableFactorAST
{
	std::string name;
public:
	TableNameAST(const std::string& name) :name(name) {}
};

class TableQueryAST final :public TableFactorAST
{
	std::string alias;
	std::unique_ptr<SubqueryAST> subq;
public:
	TableQueryAST(const std::string& alias, std::unique_ptr<SubqueryAST> subq) :
		alias(alias), subq(std::move(subq)) {}

};

class ParenTableRefAST final :public TableFactorAST
{
	std::unique_ptr<TableRefsAST> refs;
public:
	ParenTableRefAST(std::unique_ptr<TableRefsAST> refs) :
		refs(std::move(refs)) {}
};

class OnJoinCondAST final :public JoinCondAST
{
	std::unique_ptr<ExprAST> cond;
public:
	OnJoinCondAST(std::unique_ptr<ExprAST> cond) :
		cond(std::move(cond)) {}
};

class UsingJoinCondAST final :public JoinCondAST
{
public:
	std::vector<std::unique_ptr<TablecolAST>> cols;
	UsingJoinCondAST(std::vector<std::unique_ptr<TablecolAST>> cols) :
		cols(std::move(cols)) {}
};

class TRIJAST final :public JoinTableAST
{
	std::unique_ptr<TableRefAST> ref;
	std::unique_ptr<TableFactorAST> factor;
	std::unique_ptr<JoinCondAST> cond;
public:
	bool innerflag = false;
	bool crossflag = false;
	TRIJAST(std::unique_ptr<TableRefAST> ref, std::unique_ptr<TableFactorAST> factor) :
		ref(std::move(ref)), factor(std::move(factor)), cond(nullptr) {}
	TRIJAST(std::unique_ptr<TableRefAST> ref, std::unique_ptr<TableFactorAST> factor, std::unique_ptr<JoinCondAST> cond) :
		ref(std::move(ref)), factor(std::move(factor)), cond(std::move(cond)) {}
};

class TRLROJAST final :public JoinTableAST
{
	std::unique_ptr<TableRefAST> lhs;
	std::unique_ptr<TableRefAST> rhs;
	std::unique_ptr<JoinCondAST> cond;
public:
	bool leftflag = false;
	bool rightflag = false;
	bool outerflag = false;
	TRLROJAST(std::unique_ptr<TableRefAST> lhs, std::unique_ptr<TableRefAST> rhs, std::unique_ptr<JoinCondAST> cond) :
		lhs(std::move(lhs)), rhs(std::move(rhs)), cond(std::move(cond)) {}
};

class TRNLROJAST final :public JoinTableAST
{
	std::unique_ptr<TableRefAST> ref;
	std::unique_ptr<TableFactorAST> factor;
public:
	bool leftflag = false;
	bool rightflag = false;
	bool outerflag = false;
	TRNLROJAST(std::unique_ptr<TableRefAST> ref, std::unique_ptr<TableFactorAST> factor) :
		ref(std::move(ref)), factor(std::move(factor)) {}
};


class SubqueryAST final :public SimpleExprAST
{
	//	handle select * from CASE
	bool distinct_flag = false;
	std::vector<std::unique_ptr<ExprAST>> exprs;

	bool from_flag = false;
	std::vector<std::unique_ptr<TableRefsAST>> tbrefs;

	bool where_flag = false;  
	std::unique_ptr<ExprAST> wherecond;

	bool having_flag = false;  
	std::unique_ptr<ExprAST> havingcond;

	bool group_flag = false;  bool group_ASC = true;
	std::vector<table_col> groupby_col_name;

	bool order_flag = false;  bool order_ASC = true;
	std::vector<table_col> orderby_col_name;

public:
	SubqueryAST(bool distinct_flag, std::vector<std::unique_ptr<ExprAST>> exprs,
		bool from_flag, std::vector<std::unique_ptr<TableRefsAST>> tbrefs,
		bool where_flag, std::unique_ptr<ExprAST> wherecond,
		bool having_flag, std::unique_ptr<ExprAST> havingcond,
		bool group_flag, bool group_ASC,std::vector<table_col> groupby_col_name,
		bool order_flag, bool order_ASC,std::vector<table_col> orderby_col_name
	) :
		distinct_flag(distinct_flag), exprs(std::move(exprs)),
		from_flag(from_flag), tbrefs(std::move(tbrefs)),
		where_flag(where_flag), wherecond(std::move(wherecond)),
		having_flag(having_flag), havingcond(std::move(havingcond)),
		group_flag(group_flag), group_ASC(group_ASC),groupby_col_name(std::move(groupby_col_name)),
		order_flag(order_flag), order_ASC(order_ASC),orderby_col_name(std::move(orderby_col_name))
	{}
};

class ExistsSubqueryAST final :public SimpleExprAST
{
	std::unique_ptr<SubqueryAST> subquery;
public:
	ExistsSubqueryAST(std::unique_ptr<SubqueryAST> subquery) :subquery(std::move(subquery)) {}
};




class StatementAST
{
public:
	virtual ~StatementAST() = default;
};



class CreateAST :public StatementAST
{
	;
};

class CreateTableAST :public CreateAST
{
	;
};



class CreateTableSimpleAST :public CreateTableAST
{
	std::string table_name; 
	std::vector<std::unique_ptr<ColdefAST>> create_defs;
public:
	CreateTableSimpleAST(const std::string table_name,std::vector<std::unique_ptr<ColdefAST>> create_defs) :
		table_name(table_name), create_defs(std::move(create_defs)) {}
	~CreateTableSimpleAST() = default;
};

class CreateTableSelectAST :public CreateTableAST
{
	std::string table_name;
public:
	CreateTableSelectAST(const std::string& table_name) :
		table_name(table_name) {}
};

class CreateTableLikeAST :public CreateTableAST
{
	std::string table_name;
	std::string old_name;
public:
	CreateTableLikeAST(const std::string& table_name, const std::string& old_name) :
		table_name(table_name), old_name(old_name) {}
};

class CreateIndexAST :public CreateAST
{
	std::string index_name;
	std::string table_name;
	std::string col_name;
public:
	CreateIndexAST(const std::string& index_name, const std::string& table_name, const std::string& col_name) :
		index_name(index_name), table_name(table_name), col_name(col_name) {}
};

class DropAST :public StatementAST {};

class DropTableAST :public DropAST
{
	std::vector<std::string> table_list;

public:
	DropTableAST(std::vector<std::string> table_list) : table_list(std::move(table_list)) {}
};

class DropIndexAST :public DropAST
{
	std::string index_name;
	std::string table_name;
public:
	DropIndexAST(const std::string& index_name, const std::string& table_name) :
		index_name(std::move(index_name)), table_name(std::move(table_name)) {}
};

class InsertAST :public StatementAST
{
	std::string table_name;
	std::vector<std::string> col_name;
	std::vector<std::unique_ptr<ExprAST>> value_list;
public:
	InsertAST(const std::string& table_name, std::vector<std::string>col_name, std::vector<std::unique_ptr<ExprAST>> value_list) :
		table_name(table_name), col_name(std::move(col_name)), value_list(std::move(value_list)) {}
};

class DeleteAST :public StatementAST
{
	std::string table_name;
	std::unique_ptr< ExprAST> where_condition;
public:
	DeleteAST(const std::string& table_name, std::unique_ptr<ExprAST>where_condition) :
		table_name(table_name), where_condition(std::move(where_condition)) {}
};

class SelectAST :public StatementAST
{
	std::unique_ptr<SubqueryAST> subquery;
};


void init_scanner();



std::unique_ptr<ExprAST> ParseExprAST();
std::unique_ptr<ExpAST> ParseExpAST();
std::unique_ptr<BooleanPrimaryAST> ParseBPAST();
std::unique_ptr<PredicateAST> ParsePredicateAST();
std::unique_ptr<BitExprAST> ParseBitExprAST();
std::unique_ptr<BitExpAST> ParseBitExpAST();
std::unique_ptr<BitExAST> ParseBitExAST();
std::unique_ptr<SimpleExprAST> ParseSEAST();
std::unique_ptr<LiteralAST> ParseLiteralAST();
std::unique_ptr<StringLiteralAST> ParseStringLiteralAST();
std::unique_ptr<IntLiteralAST> ParseIntLiteralAST();
std::unique_ptr<DoubleLiteralAST> ParseDoubleLiteralAST();
std::unique_ptr<TablecolAST> ParseTablecolAST();
std::unique_ptr<ParenExprAST> ParseParenExprAST();
std::unique_ptr<IdAST> ParseIdAST();
std::unique_ptr<CallAST> ParseCallAST();
std::unique_ptr<ExistsSubqueryAST> ParseExistsSubqueryAST();
std::unique_ptr<SubqueryAST> ParseSubqueryAST();
std::unique_ptr<CreateTableSimpleAST> ParseCreateTableSimpleAST();
std::unique_ptr<ColdefAST> ParseColdefAST();
std::unique_ptr<OnJoinCondAST> ParseOnJoinCondAST();
std::unique_ptr<UsingJoinCondAST> ParseUsingJoinCondAST();





#endif // !llvmsql_h




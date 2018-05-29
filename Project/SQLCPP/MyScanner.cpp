#include<cstdio>
#include<cstdlib>
#include<string>
#include<algorithm>
#include<map>
#include<vector>
#include<exception>
#include<regex>
#include"llvmsql.h"

using namespace std;

#define LookAheadLen 6
int LookAhead[LookAheadLen] = { ' ',' ',' ',' ',' ',' ' };



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

// ignore blank
// ignore comment
// to store current token-value
std::string string_literal; // ' ""
int int_literal;    // 先判断是否是 int
double double_literal; // 后判断是否是 double
int symbol_mark;
std::string IdentifierStr;
static int scanner_status = blank;

vector<string> reserved_dict
{ "ACCESSIBLE","ACCOUNT","ACTION","ADD","AFTER","AGAINST","AGGREGATE","ALGORITHM","ALL","ALTER",
"ALWAYS","ANALYSE","ANALYZE","AND","ANY","AS","ASC","ASCII","ASENSITIVE","AT",
"AUTOEXTEND_SIZE","AUTO_INCREMENT","AVG","AVG_ROW_LENGTH","BACKUP","BDB","BEFORE","BEGIN","BERKELEYDB","BETWEEN",
"BIGINT","BINARY","BINLOG","BIT","BLOB","BLOCK","BOOL","BOOLEAN","BOTH","BTREE",
"BY","BYTE","CACHE","CALL","CASCADE","CASCADED","CASE","CATALOG_NAME","CHAIN","CHANGE",
"CHANGED","CHANNEL","CHAR","CHARACTER","CHARSET","CHECK","CHECKSUM","CIPHER","CLASS_ORIGIN","CLIENT",
"CLOSE","COALESCE","CODE","COLLATE","COLLATION","COLUMN","COLUMNS","COLUMN_FORMAT","COLUMN_NAME","COMMENT",
"COMMIT","COMMITTED","COMPACT","COMPLETION","COMPRESSED","COMPRESSION","CONCURRENT","CONDITION","CONNECTION","CONSISTENT",
"CONSTRAINT","CONSTRAINT_CATALOG","CONSTRAINT_NAME","CONSTRAINT_SCHEMA","CONTAINS","CONTEXT","CONTINUE","CONVERT","CPU","CREATE",
"CROSS","CUBE","CURRENT","CURRENT_DATE","CURRENT_TIME","CURRENT_TIMESTAMP","CURRENT_USER","CURSOR","CURSOR_NAME","DATA",
"DATABASE","DATABASES","DATAFILE","DATE","DATETIME","DAY","DAY_HOUR","DAY_MICROSECOND","DAY_MINUTE","DAY_SECOND",
"DEALLOCATE","DEC","DECIMAL","DECLARE","DEFAULT","DEFAULT_AUTH","DEFINER","DELAYED","DELAY_KEY_WRITE","DELETE",
"DESC","DESCRIBE","DES_KEY_FILE","DETERMINISTIC","DIAGNOSTICS","DIRECTORY","DISABLE","DISCARD","DISK","DISTINCT",
"DISTINCTROW","DIV","DO","DOUBLE","DROP","DUAL","DUMPFILE","DUPLICATE","DYNAMIC","EACH",
"ELSE","ELSEIF","ENABLE","ENCLOSED","ENCRYPTION","END","ENDS","ENGINE","ENGINES","ENUM",
"ERROR","ERRORS","ESCAPE","ESCAPED","EVENT","EVENTS","EVERY","EXCHANGE","EXECUTE","EXISTS",
"EXIT","EXPANSION","EXPIRE","EXPLAIN","EXPORT","EXTENDED","EXTENT_SIZE","FALSE","FAST","FAULTS",
"FETCH","FIELDS","FILE","FILE_BLOCK_SIZE","FILTER","FIRST","FIXED","FLOAT","FLOAT4","FLOAT8",
"FLUSH","FOLLOWS","FOR","FORCE","FOREIGN","FORMAT","FOUND","FRAC_SECOND","FROM","FULL",
"FULLTEXT","FUNCTION","GENERAL","GENERATED","GEOMETRY","GEOMETRYCOLLECTION","GET","GET_FORMAT","GLOBAL","GRANT",
"GRANTS","GROUP","GROUP_REPLICATION","HANDLER","HASH","HAVING","HELP","HIGH_PRIORITY","HOST","HOSTS",
"HOUR","HOUR_MICROSECOND","HOUR_MINUTE","HOUR_SECOND","IDENTIFIED","IF","IGNORE","IGNORE_SERVER_IDS","IMPORT","IN",
"INDEX","INDEXES","INFILE","INITIAL_SIZE","INNER","INNODB","INOUT","INSENSITIVE","INSERT","INSERT_METHOD",
"INSTALL","INSTANCE","INT","INT1","INT2","INT3","INT4","INT8","INTEGER","INTERVAL",
"INTO","INVOKER","IO","IO_AFTER_GTIDS","IO_BEFORE_GTIDS","IO_THREAD","IPC","IS","ISOLATION","ISSUER",
"ITERATE","JOIN","JSON","KEY","KEYS","KEY_BLOCK_SIZE","KILL","LANGUAGE","LAST","LEADING",
"LEAVE","LEAVES","LEFT","LESS","LEVEL","LIKE","LIMIT","LINEAR","LINES","LINESTRING",
"LIST","LOAD","LOCAL","LOCALTIME","LOCALTIMESTAMP","LOCK","LOCKS","LOGFILE","LOGS","LONG",
"LONGBLOB","LONGTEXT","LOOP","LOW_PRIORITY","MASTER","MASTER_AUTO_POSITION","MASTER_BIND","MASTER_CONNECT_RETRY","MASTER_DELAY","MASTER_HEARTBEAT_PERIOD",
"MASTER_HOST","MASTER_LOG_FILE","MASTER_LOG_POS","MASTER_PASSWORD","MASTER_PORT","MASTER_RETRY_COUNT","MASTER_SERVER_ID","MASTER_SSL","MASTER_SSL_CA","MASTER_SSL_CAPATH",
"MASTER_SSL_CERT","MASTER_SSL_CIPHER","MASTER_SSL_CRL","MASTER_SSL_CRLPATH","MASTER_SSL_KEY","MASTER_SSL_VERIFY_SERVER_CERT","MASTER_TLS_VERSION","MASTER_USER","MATCH","MAXVALUE",
"MAX_CONNECTIONS_PER_HOUR","MAX_QUERIES_PER_HOUR","MAX_ROWS","MAX_SIZE","MAX_STATEMENT_TIME","MAX_UPDATES_PER_HOUR","MAX_USER_CONNECTIONS","MEDIUM","MEDIUMBLOB","MEDIUMINT",
"MEDIUMTEXT","MEMORY","MERGE","MESSAGE_TEXT","MICROSECOND","MIDDLEINT","MIGRATE","MINUTE","MINUTE_MICROSECOND","MINUTE_SECOND",
"MIN_ROWS","MOD","MODE","MODIFIES","MODIFY","MONTH","MULTILINESTRING","MULTIPOINT","MULTIPOLYGON","MUTEX",
"MYSQL_ERRNO","NAME","NAMES","NATIONAL","NATURAL","NCHAR","NDB","NDBCLUSTER","NEVER","NEW",
"NEXT","NO","NODEGROUP","NONBLOCKING","NONE","NOT","NO_WAIT","NO_WRITE_TO_BINLOG","NULL","NUMBER",
"NUMERIC","NVARCHAR","OFFSET","OLD_PASSWORD","ON","ONE","ONLY","OPEN","OPTIMIZE","OPTIMIZER_COSTS",
"OPTION","OPTIONALLY","OPTIONS","OR","ORDER","OUT","OUTER","OUTFILE","OWNER","PACK_KEYS",
"PAGE","PARSER","PARSE_GCOL_EXPR","PARTIAL","PARTITION","PARTITIONING","PARTITIONS","PASSWORD","PHASE","PLUGIN",
"PLUGINS","PLUGIN_DIR","POINT","POLYGON","PORT","PRECEDES","PRECISION","PREPARE","PRESERVE","PREV",
"PRIMARY","PRIVILEGES","PROCEDURE","PROCESSLIST","PROFILE","PROFILES","PROXY","PURGE","QUARTER","QUERY",
"QUICK","RANGE","READ","READS","READ_ONLY","READ_WRITE","REAL","REBUILD","RECOVER","REDOFILE",
"REDO_BUFFER_SIZE","REDUNDANT","REFERENCES","REGEXP","RELAY","RELAYLOG","RELAY_LOG_FILE","RELAY_LOG_POS","RELAY_THREAD","RELEASE",
"RELOAD","REMOVE","RENAME","REORGANIZE","REPAIR","REPEAT","REPEATABLE","REPLACE","REPLICATE_DO_DB","REPLICATE_DO_TABLE",
"REPLICATE_IGNORE_DB","REPLICATE_IGNORE_TABLE","REPLICATE_REWRITE_DB","REPLICATE_WILD_DO_TABLE","REPLICATE_WILD_IGNORE_TABLE","REPLICATION","REQUIRE","RESET","RESIGNAL","RESTORE",
"RESTRICT","RESUME","RETURN","RETURNED_SQLSTATE","RETURNS","REVERSE","REVOKE","RIGHT","RLIKE","ROLLBACK",
"ROLLUP","ROTATE","ROUTINE","ROW","ROWS","ROW_COUNT","ROW_FORMAT","RTREE","SAVEPOINT","SCHEDULE",
"SCHEMA","SCHEMAS","SCHEMA_NAME","SECOND","SECOND_MICROSECOND","SECURITY","SELECT","SENSITIVE","SEPARATOR","SERIAL",
"SERIALIZABLE","SERVER","SESSION","SET","SHARE","SHOW","SHUTDOWN","SIGNAL","SIGNED","SIMPLE",
"SLAVE","SLOW","SMALLINT","SNAPSHOT","SOCKET","SOME","SONAME","SOUNDS","SOURCE","SPATIAL",
"SPECIFIC","SQL","SQLEXCEPTION","SQLSTATE","SQLWARNING","SQL_AFTER_GTIDS","SQL_AFTER_MTS_GAPS","SQL_BEFORE_GTIDS","SQL_BIG_RESULT","SQL_BUFFER_RESULT",
"SQL_CACHE","SQL_CALC_FOUND_ROWS","SQL_NO_CACHE","SQL_SMALL_RESULT","SQL_THREAD","SQL_TSI_DAY","SQL_TSI_FRAC_SECOND","SQL_TSI_HOUR","SQL_TSI_MINUTE","SQL_TSI_MONTH",
"SQL_TSI_QUARTER","SQL_TSI_SECOND","SQL_TSI_WEEK","SQL_TSI_YEAR","SSL","STACKED","START","STARTING","STARTS","STATS_AUTO_RECALC",
"STATS_PERSISTENT","STATS_SAMPLE_PAGES","STATUS","STOP","STORAGE","STORED","STRAIGHT_JOIN","STRING","STRIPED","SUBCLASS_ORIGIN",
"SUBJECT","SUBPARTITION","SUBPARTITIONS","SUPER","SUSPEND","SWAPS","SWITCHES","TABLE","TABLES","TABLESPACE",
"TABLE_CHECKSUM","TABLE_NAME","TEMPORARY","TEMPTABLE","TERMINATED","TEXT","THAN","THEN","TIME","TIMESTAMP",
"TIMESTAMPADD","TIMESTAMPDIFF","TINYBLOB","TINYINT","TINYTEXT","TO","TRAILING","TRANSACTION","TRIGGER","TRIGGERS",
"TRUE","TRUNCATE","TYPE","TYPES","UNCOMMITTED","UNDEFINED","UNDO","UNDOFILE","UNDO_BUFFER_SIZE","UNICODE",
"UNINSTALL","UNION","UNIQUE","UNKNOWN","UNLOCK","UNSIGNED","UNTIL","UPDATE","UPGRADE","USAGE",
"USE","USER","USER_RESOURCES","USE_FRM","USING","UTC_DATE","UTC_TIME","UTC_TIMESTAMP","VALIDATION","VALUE",
"VALUES","VARBINARY","VARCHAR","VARCHARACTER","VARIABLES","VARYING","VIEW","VIRTUAL","WAIT","WARNINGS",
"WEEK","WEIGHT_STRING","WHEN","WHERE","WHILE","WITH","WITHOUT","WORK","WRAPPER","WRITE",
"X509","XA","XID","XML","XOR","YEAR","YEAR_MONTH","ZEROFILL" };

map<string, int> reserved_map;

void init_scanner()
{
	reserved_map.insert(map<string, int>::value_type("ACCESSIBLE", -1));
	reserved_map.insert(map<string, int>::value_type("ACCOUNT", -2));
	reserved_map.insert(map<string, int>::value_type("ACTION", -3));
	reserved_map.insert(map<string, int>::value_type("ADD", -4));
	reserved_map.insert(map<string, int>::value_type("AFTER", -5));
	reserved_map.insert(map<string, int>::value_type("AGAINST", -6));
	reserved_map.insert(map<string, int>::value_type("AGGREGATE", -7));
	reserved_map.insert(map<string, int>::value_type("ALGORITHM", -8));
	reserved_map.insert(map<string, int>::value_type("ALL", -9));
	reserved_map.insert(map<string, int>::value_type("ALTER", -10));
	reserved_map.insert(map<string, int>::value_type("ALWAYS", -11));
	reserved_map.insert(map<string, int>::value_type("ANALYSE", -12));
	reserved_map.insert(map<string, int>::value_type("ANALYZE", -13));
	reserved_map.insert(map<string, int>::value_type("AND", -14));
	reserved_map.insert(map<string, int>::value_type("ANY", -15));
	reserved_map.insert(map<string, int>::value_type("AS", -16));
	reserved_map.insert(map<string, int>::value_type("ASC", -17));
	reserved_map.insert(map<string, int>::value_type("ASCII", -18));
	reserved_map.insert(map<string, int>::value_type("ASENSITIVE", -19));
	reserved_map.insert(map<string, int>::value_type("AT", -20));
	reserved_map.insert(map<string, int>::value_type("AUTOEXTEND_SIZE", -21));
	reserved_map.insert(map<string, int>::value_type("AUTO_INCREMENT", -22));
	reserved_map.insert(map<string, int>::value_type("AVG", -23));
	reserved_map.insert(map<string, int>::value_type("AVG_ROW_LENGTH", -24));
	reserved_map.insert(map<string, int>::value_type("BACKUP", -25));
	reserved_map.insert(map<string, int>::value_type("BDB", -26));
	reserved_map.insert(map<string, int>::value_type("BEFORE", -27));
	reserved_map.insert(map<string, int>::value_type("BEGIN", -28));
	reserved_map.insert(map<string, int>::value_type("BERKELEYDB", -29));
	reserved_map.insert(map<string, int>::value_type("BETWEEN", -30));
	reserved_map.insert(map<string, int>::value_type("BIGINT", -31));
	reserved_map.insert(map<string, int>::value_type("BINARY", -32));
	reserved_map.insert(map<string, int>::value_type("BINLOG", -33));
	reserved_map.insert(map<string, int>::value_type("BIT", -34));
	reserved_map.insert(map<string, int>::value_type("BLOB", -35));
	reserved_map.insert(map<string, int>::value_type("BLOCK", -36));
	reserved_map.insert(map<string, int>::value_type("BOOL", -37));
	reserved_map.insert(map<string, int>::value_type("BOOLEAN", -38));
	reserved_map.insert(map<string, int>::value_type("BOTH", -39));
	reserved_map.insert(map<string, int>::value_type("BTREE", -40));
	reserved_map.insert(map<string, int>::value_type("BY", -41));
	reserved_map.insert(map<string, int>::value_type("BYTE", -42));
	reserved_map.insert(map<string, int>::value_type("CACHE", -43));
	reserved_map.insert(map<string, int>::value_type("CALL", -44));
	reserved_map.insert(map<string, int>::value_type("CASCADE", -45));
	reserved_map.insert(map<string, int>::value_type("CASCADED", -46));
	reserved_map.insert(map<string, int>::value_type("CASE", -47));
	reserved_map.insert(map<string, int>::value_type("CATALOG_NAME", -48));
	reserved_map.insert(map<string, int>::value_type("CHAIN", -49));
	reserved_map.insert(map<string, int>::value_type("CHANGE", -50));
	reserved_map.insert(map<string, int>::value_type("CHANGED", -51));
	reserved_map.insert(map<string, int>::value_type("CHANNEL", -52));
	reserved_map.insert(map<string, int>::value_type("CHAR", -53));
	reserved_map.insert(map<string, int>::value_type("CHARACTER", -54));
	reserved_map.insert(map<string, int>::value_type("CHARSET", -55));
	reserved_map.insert(map<string, int>::value_type("CHECK", -56));
	reserved_map.insert(map<string, int>::value_type("CHECKSUM", -57));
	reserved_map.insert(map<string, int>::value_type("CIPHER", -58));
	reserved_map.insert(map<string, int>::value_type("CLASS_ORIGIN", -59));
	reserved_map.insert(map<string, int>::value_type("CLIENT", -60));
	reserved_map.insert(map<string, int>::value_type("CLOSE", -61));
	reserved_map.insert(map<string, int>::value_type("COALESCE", -62));
	reserved_map.insert(map<string, int>::value_type("CODE", -63));
	reserved_map.insert(map<string, int>::value_type("COLLATE", -64));
	reserved_map.insert(map<string, int>::value_type("COLLATION", -65));
	reserved_map.insert(map<string, int>::value_type("COLUMN", -66));
	reserved_map.insert(map<string, int>::value_type("COLUMNS", -67));
	reserved_map.insert(map<string, int>::value_type("COLUMN_FORMAT", -68));
	reserved_map.insert(map<string, int>::value_type("COLUMN_NAME", -69));
	reserved_map.insert(map<string, int>::value_type("COMMENT", -70));
	reserved_map.insert(map<string, int>::value_type("COMMIT", -71));
	reserved_map.insert(map<string, int>::value_type("COMMITTED", -72));
	reserved_map.insert(map<string, int>::value_type("COMPACT", -73));
	reserved_map.insert(map<string, int>::value_type("COMPLETION", -74));
	reserved_map.insert(map<string, int>::value_type("COMPRESSED", -75));
	reserved_map.insert(map<string, int>::value_type("COMPRESSION", -76));
	reserved_map.insert(map<string, int>::value_type("CONCURRENT", -77));
	reserved_map.insert(map<string, int>::value_type("CONDITION", -78));
	reserved_map.insert(map<string, int>::value_type("CONNECTION", -79));
	reserved_map.insert(map<string, int>::value_type("CONSISTENT", -80));
	reserved_map.insert(map<string, int>::value_type("CONSTRAINT", -81));
	reserved_map.insert(map<string, int>::value_type("CONSTRAINT_CATALOG", -82));
	reserved_map.insert(map<string, int>::value_type("CONSTRAINT_NAME", -83));
	reserved_map.insert(map<string, int>::value_type("CONSTRAINT_SCHEMA", -84));
	reserved_map.insert(map<string, int>::value_type("CONTAINS", -85));
	reserved_map.insert(map<string, int>::value_type("CONTEXT", -86));
	reserved_map.insert(map<string, int>::value_type("CONTINUE", -87));
	reserved_map.insert(map<string, int>::value_type("CONVERT", -88));
	reserved_map.insert(map<string, int>::value_type("CPU", -89));
	reserved_map.insert(map<string, int>::value_type("CREATE", -90));
	reserved_map.insert(map<string, int>::value_type("CROSS", -91));
	reserved_map.insert(map<string, int>::value_type("CUBE", -92));
	reserved_map.insert(map<string, int>::value_type("CURRENT", -93));
	reserved_map.insert(map<string, int>::value_type("CURRENT_DATE", -94));
	reserved_map.insert(map<string, int>::value_type("CURRENT_TIME", -95));
	reserved_map.insert(map<string, int>::value_type("CURRENT_TIMESTAMP", -96));
	reserved_map.insert(map<string, int>::value_type("CURRENT_USER", -97));
	reserved_map.insert(map<string, int>::value_type("CURSOR", -98));
	reserved_map.insert(map<string, int>::value_type("CURSOR_NAME", -99));
	reserved_map.insert(map<string, int>::value_type("DATA", -100));
	reserved_map.insert(map<string, int>::value_type("DATABASE", -101));
	reserved_map.insert(map<string, int>::value_type("DATABASES", -102));
	reserved_map.insert(map<string, int>::value_type("DATAFILE", -103));
	reserved_map.insert(map<string, int>::value_type("DATE", -104));
	reserved_map.insert(map<string, int>::value_type("DATETIME", -105));
	reserved_map.insert(map<string, int>::value_type("DAY", -106));
	reserved_map.insert(map<string, int>::value_type("DAY_HOUR", -107));
	reserved_map.insert(map<string, int>::value_type("DAY_MICROSECOND", -108));
	reserved_map.insert(map<string, int>::value_type("DAY_MINUTE", -109));
	reserved_map.insert(map<string, int>::value_type("DAY_SECOND", -110));
	reserved_map.insert(map<string, int>::value_type("DEALLOCATE", -111));
	reserved_map.insert(map<string, int>::value_type("DEC", -112));
	reserved_map.insert(map<string, int>::value_type("DECIMAL", -113));
	reserved_map.insert(map<string, int>::value_type("DECLARE", -114));
	reserved_map.insert(map<string, int>::value_type("DEFAULT", -115));
	reserved_map.insert(map<string, int>::value_type("DEFAULT_AUTH", -116));
	reserved_map.insert(map<string, int>::value_type("DEFINER", -117));
	reserved_map.insert(map<string, int>::value_type("DELAYED", -118));
	reserved_map.insert(map<string, int>::value_type("DELAY_KEY_WRITE", -119));
	reserved_map.insert(map<string, int>::value_type("DELETE", -120));
	reserved_map.insert(map<string, int>::value_type("DESC", -121));
	reserved_map.insert(map<string, int>::value_type("DESCRIBE", -122));
	reserved_map.insert(map<string, int>::value_type("DES_KEY_FILE", -123));
	reserved_map.insert(map<string, int>::value_type("DETERMINISTIC", -124));
	reserved_map.insert(map<string, int>::value_type("DIAGNOSTICS", -125));
	reserved_map.insert(map<string, int>::value_type("DIRECTORY", -126));
	reserved_map.insert(map<string, int>::value_type("DISABLE", -127));
	reserved_map.insert(map<string, int>::value_type("DISCARD", -128));
	reserved_map.insert(map<string, int>::value_type("DISK", -129));
	reserved_map.insert(map<string, int>::value_type("DISTINCT", -130));
	reserved_map.insert(map<string, int>::value_type("DISTINCTROW", -131));
	reserved_map.insert(map<string, int>::value_type("DIV", -132));
	reserved_map.insert(map<string, int>::value_type("DO", -133));
	reserved_map.insert(map<string, int>::value_type("DOUBLE", -134));
	reserved_map.insert(map<string, int>::value_type("DROP", -135));
	reserved_map.insert(map<string, int>::value_type("DUAL", -136));
	reserved_map.insert(map<string, int>::value_type("DUMPFILE", -137));
	reserved_map.insert(map<string, int>::value_type("DUPLICATE", -138));
	reserved_map.insert(map<string, int>::value_type("DYNAMIC", -139));
	reserved_map.insert(map<string, int>::value_type("EACH", -140));
	reserved_map.insert(map<string, int>::value_type("ELSE", -141));
	reserved_map.insert(map<string, int>::value_type("ELSEIF", -142));
	reserved_map.insert(map<string, int>::value_type("ENABLE", -143));
	reserved_map.insert(map<string, int>::value_type("ENCLOSED", -144));
	reserved_map.insert(map<string, int>::value_type("ENCRYPTION", -145));
	reserved_map.insert(map<string, int>::value_type("END", -146));
	reserved_map.insert(map<string, int>::value_type("ENDS", -147));
	reserved_map.insert(map<string, int>::value_type("ENGINE", -148));
	reserved_map.insert(map<string, int>::value_type("ENGINES", -149));
	reserved_map.insert(map<string, int>::value_type("ENUM", -150));
	reserved_map.insert(map<string, int>::value_type("ERROR", -151));
	reserved_map.insert(map<string, int>::value_type("ERRORS", -152));
	reserved_map.insert(map<string, int>::value_type("ESCAPE", -153));
	reserved_map.insert(map<string, int>::value_type("ESCAPED", -154));
	reserved_map.insert(map<string, int>::value_type("EVENT", -155));
	reserved_map.insert(map<string, int>::value_type("EVENTS", -156));
	reserved_map.insert(map<string, int>::value_type("EVERY", -157));
	reserved_map.insert(map<string, int>::value_type("EXCHANGE", -158));
	reserved_map.insert(map<string, int>::value_type("EXECUTE", -159));
	reserved_map.insert(map<string, int>::value_type("EXISTS", -160));
	reserved_map.insert(map<string, int>::value_type("EXIT", -161));
	reserved_map.insert(map<string, int>::value_type("EXPANSION", -162));
	reserved_map.insert(map<string, int>::value_type("EXPIRE", -163));
	reserved_map.insert(map<string, int>::value_type("EXPLAIN", -164));
	reserved_map.insert(map<string, int>::value_type("EXPORT", -165));
	reserved_map.insert(map<string, int>::value_type("EXTENDED", -166));
	reserved_map.insert(map<string, int>::value_type("EXTENT_SIZE", -167));
	reserved_map.insert(map<string, int>::value_type("FALSE", -168));
	reserved_map.insert(map<string, int>::value_type("FAST", -169));
	reserved_map.insert(map<string, int>::value_type("FAULTS", -170));
	reserved_map.insert(map<string, int>::value_type("FETCH", -171));
	reserved_map.insert(map<string, int>::value_type("FIELDS", -172));
	reserved_map.insert(map<string, int>::value_type("FILE", -173));
	reserved_map.insert(map<string, int>::value_type("FILE_BLOCK_SIZE", -174));
	reserved_map.insert(map<string, int>::value_type("FILTER", -175));
	reserved_map.insert(map<string, int>::value_type("FIRST", -176));
	reserved_map.insert(map<string, int>::value_type("FIXED", -177));
	reserved_map.insert(map<string, int>::value_type("FLOAT", -178));
	reserved_map.insert(map<string, int>::value_type("FLOAT4", -179));
	reserved_map.insert(map<string, int>::value_type("FLOAT8", -180));
	reserved_map.insert(map<string, int>::value_type("FLUSH", -181));
	reserved_map.insert(map<string, int>::value_type("FOLLOWS", -182));
	reserved_map.insert(map<string, int>::value_type("FOR", -183));
	reserved_map.insert(map<string, int>::value_type("FORCE", -184));
	reserved_map.insert(map<string, int>::value_type("FOREIGN", -185));
	reserved_map.insert(map<string, int>::value_type("FORMAT", -186));
	reserved_map.insert(map<string, int>::value_type("FOUND", -187));
	reserved_map.insert(map<string, int>::value_type("FRAC_SECOND", -188));
	reserved_map.insert(map<string, int>::value_type("FROM", -189));
	reserved_map.insert(map<string, int>::value_type("FULL", -190));
	reserved_map.insert(map<string, int>::value_type("FULLTEXT", -191));
	reserved_map.insert(map<string, int>::value_type("FUNCTION", -192));
	reserved_map.insert(map<string, int>::value_type("GENERAL", -193));
	reserved_map.insert(map<string, int>::value_type("GENERATED", -194));
	reserved_map.insert(map<string, int>::value_type("GEOMETRY", -195));
	reserved_map.insert(map<string, int>::value_type("GEOMETRYCOLLECTION", -196));
	reserved_map.insert(map<string, int>::value_type("GET", -197));
	reserved_map.insert(map<string, int>::value_type("GET_FORMAT", -198));
	reserved_map.insert(map<string, int>::value_type("GLOBAL", -199));
	reserved_map.insert(map<string, int>::value_type("GRANT", -200));
	reserved_map.insert(map<string, int>::value_type("GRANTS", -201));
	reserved_map.insert(map<string, int>::value_type("GROUP", -202));
	reserved_map.insert(map<string, int>::value_type("GROUP_REPLICATION", -203));
	reserved_map.insert(map<string, int>::value_type("HANDLER", -204));
	reserved_map.insert(map<string, int>::value_type("HASH", -205));
	reserved_map.insert(map<string, int>::value_type("HAVING", -206));
	reserved_map.insert(map<string, int>::value_type("HELP", -207));
	reserved_map.insert(map<string, int>::value_type("HIGH_PRIORITY", -208));
	reserved_map.insert(map<string, int>::value_type("HOST", -209));
	reserved_map.insert(map<string, int>::value_type("HOSTS", -210));
	reserved_map.insert(map<string, int>::value_type("HOUR", -211));
	reserved_map.insert(map<string, int>::value_type("HOUR_MICROSECOND", -212));
	reserved_map.insert(map<string, int>::value_type("HOUR_MINUTE", -213));
	reserved_map.insert(map<string, int>::value_type("HOUR_SECOND", -214));
	reserved_map.insert(map<string, int>::value_type("IDENTIFIED", -215));
	reserved_map.insert(map<string, int>::value_type("IF", -216));
	reserved_map.insert(map<string, int>::value_type("IGNORE", -217));
	reserved_map.insert(map<string, int>::value_type("IGNORE_SERVER_IDS", -218));
	reserved_map.insert(map<string, int>::value_type("IMPORT", -219));
	reserved_map.insert(map<string, int>::value_type("IN", -220));
	reserved_map.insert(map<string, int>::value_type("INDEX", -221));
	reserved_map.insert(map<string, int>::value_type("INDEXES", -222));
	reserved_map.insert(map<string, int>::value_type("INFILE", -223));
	reserved_map.insert(map<string, int>::value_type("INITIAL_SIZE", -224));
	reserved_map.insert(map<string, int>::value_type("INNER", -225));
	reserved_map.insert(map<string, int>::value_type("INNODB", -226));
	reserved_map.insert(map<string, int>::value_type("INOUT", -227));
	reserved_map.insert(map<string, int>::value_type("INSENSITIVE", -228));
	reserved_map.insert(map<string, int>::value_type("INSERT", -229));
	reserved_map.insert(map<string, int>::value_type("INSERT_METHOD", -230));
	reserved_map.insert(map<string, int>::value_type("INSTALL", -231));
	reserved_map.insert(map<string, int>::value_type("INSTANCE", -232));
	reserved_map.insert(map<string, int>::value_type("INT", -233));
	reserved_map.insert(map<string, int>::value_type("INT1", -234));
	reserved_map.insert(map<string, int>::value_type("INT2", -235));
	reserved_map.insert(map<string, int>::value_type("INT3", -236));
	reserved_map.insert(map<string, int>::value_type("INT4", -237));
	reserved_map.insert(map<string, int>::value_type("INT8", -238));
	reserved_map.insert(map<string, int>::value_type("INTEGER", -239));
	reserved_map.insert(map<string, int>::value_type("INTERVAL", -240));
	reserved_map.insert(map<string, int>::value_type("INTO", -241));
	reserved_map.insert(map<string, int>::value_type("INVOKER", -242));
	reserved_map.insert(map<string, int>::value_type("IO", -243));
	reserved_map.insert(map<string, int>::value_type("IO_AFTER_GTIDS", -244));
	reserved_map.insert(map<string, int>::value_type("IO_BEFORE_GTIDS", -245));
	reserved_map.insert(map<string, int>::value_type("IO_THREAD", -246));
	reserved_map.insert(map<string, int>::value_type("IPC", -247));
	reserved_map.insert(map<string, int>::value_type("IS", -248));
	reserved_map.insert(map<string, int>::value_type("ISOLATION", -249));
	reserved_map.insert(map<string, int>::value_type("ISSUER", -250));
	reserved_map.insert(map<string, int>::value_type("ITERATE", -251));
	reserved_map.insert(map<string, int>::value_type("JOIN", -252));
	reserved_map.insert(map<string, int>::value_type("JSON", -253));
	reserved_map.insert(map<string, int>::value_type("KEY", -254));
	reserved_map.insert(map<string, int>::value_type("KEYS", -255));
	reserved_map.insert(map<string, int>::value_type("KEY_BLOCK_SIZE", -256));
	reserved_map.insert(map<string, int>::value_type("KILL", -257));
	reserved_map.insert(map<string, int>::value_type("LANGUAGE", -258));
	reserved_map.insert(map<string, int>::value_type("LAST", -259));
	reserved_map.insert(map<string, int>::value_type("LEADING", -260));
	reserved_map.insert(map<string, int>::value_type("LEAVE", -261));
	reserved_map.insert(map<string, int>::value_type("LEAVES", -262));
	reserved_map.insert(map<string, int>::value_type("LEFT", -263));
	reserved_map.insert(map<string, int>::value_type("LESS", -264));
	reserved_map.insert(map<string, int>::value_type("LEVEL", -265));
	reserved_map.insert(map<string, int>::value_type("LIKE", -266));
	reserved_map.insert(map<string, int>::value_type("LIMIT", -267));
	reserved_map.insert(map<string, int>::value_type("LINEAR", -268));
	reserved_map.insert(map<string, int>::value_type("LINES", -269));
	reserved_map.insert(map<string, int>::value_type("LINESTRING", -270));
	reserved_map.insert(map<string, int>::value_type("LIST", -271));
	reserved_map.insert(map<string, int>::value_type("LOAD", -272));
	reserved_map.insert(map<string, int>::value_type("LOCAL", -273));
	reserved_map.insert(map<string, int>::value_type("LOCALTIME", -274));
	reserved_map.insert(map<string, int>::value_type("LOCALTIMESTAMP", -275));
	reserved_map.insert(map<string, int>::value_type("LOCK", -276));
	reserved_map.insert(map<string, int>::value_type("LOCKS", -277));
	reserved_map.insert(map<string, int>::value_type("LOGFILE", -278));
	reserved_map.insert(map<string, int>::value_type("LOGS", -279));
	reserved_map.insert(map<string, int>::value_type("LONG", -280));
	reserved_map.insert(map<string, int>::value_type("LONGBLOB", -281));
	reserved_map.insert(map<string, int>::value_type("LONGTEXT", -282));
	reserved_map.insert(map<string, int>::value_type("LOOP", -283));
	reserved_map.insert(map<string, int>::value_type("LOW_PRIORITY", -284));
	reserved_map.insert(map<string, int>::value_type("MASTER", -285));
	reserved_map.insert(map<string, int>::value_type("MASTER_AUTO_POSITION", -286));
	reserved_map.insert(map<string, int>::value_type("MASTER_BIND", -287));
	reserved_map.insert(map<string, int>::value_type("MASTER_CONNECT_RETRY", -288));
	reserved_map.insert(map<string, int>::value_type("MASTER_DELAY", -289));
	reserved_map.insert(map<string, int>::value_type("MASTER_HEARTBEAT_PERIOD", -290));
	reserved_map.insert(map<string, int>::value_type("MASTER_HOST", -291));
	reserved_map.insert(map<string, int>::value_type("MASTER_LOG_FILE", -292));
	reserved_map.insert(map<string, int>::value_type("MASTER_LOG_POS", -293));
	reserved_map.insert(map<string, int>::value_type("MASTER_PASSWORD", -294));
	reserved_map.insert(map<string, int>::value_type("MASTER_PORT", -295));
	reserved_map.insert(map<string, int>::value_type("MASTER_RETRY_COUNT", -296));
	reserved_map.insert(map<string, int>::value_type("MASTER_SERVER_ID", -297));
	reserved_map.insert(map<string, int>::value_type("MASTER_SSL", -298));
	reserved_map.insert(map<string, int>::value_type("MASTER_SSL_CA", -299));
	reserved_map.insert(map<string, int>::value_type("MASTER_SSL_CAPATH", -300));
	reserved_map.insert(map<string, int>::value_type("MASTER_SSL_CERT", -301));
	reserved_map.insert(map<string, int>::value_type("MASTER_SSL_CIPHER", -302));
	reserved_map.insert(map<string, int>::value_type("MASTER_SSL_CRL", -303));
	reserved_map.insert(map<string, int>::value_type("MASTER_SSL_CRLPATH", -304));
	reserved_map.insert(map<string, int>::value_type("MASTER_SSL_KEY", -305));
	reserved_map.insert(map<string, int>::value_type("MASTER_SSL_VERIFY_SERVER_CERT", -306));
	reserved_map.insert(map<string, int>::value_type("MASTER_TLS_VERSION", -307));
	reserved_map.insert(map<string, int>::value_type("MASTER_USER", -308));
	reserved_map.insert(map<string, int>::value_type("MATCH", -309));
	reserved_map.insert(map<string, int>::value_type("MAXVALUE", -310));
	reserved_map.insert(map<string, int>::value_type("MAX_CONNECTIONS_PER_HOUR", -311));
	reserved_map.insert(map<string, int>::value_type("MAX_QUERIES_PER_HOUR", -312));
	reserved_map.insert(map<string, int>::value_type("MAX_ROWS", -313));
	reserved_map.insert(map<string, int>::value_type("MAX_SIZE", -314));
	reserved_map.insert(map<string, int>::value_type("MAX_STATEMENT_TIME", -315));
	reserved_map.insert(map<string, int>::value_type("MAX_UPDATES_PER_HOUR", -316));
	reserved_map.insert(map<string, int>::value_type("MAX_USER_CONNECTIONS", -317));
	reserved_map.insert(map<string, int>::value_type("MEDIUM", -318));
	reserved_map.insert(map<string, int>::value_type("MEDIUMBLOB", -319));
	reserved_map.insert(map<string, int>::value_type("MEDIUMINT", -320));
	reserved_map.insert(map<string, int>::value_type("MEDIUMTEXT", -321));
	reserved_map.insert(map<string, int>::value_type("MEMORY", -322));
	reserved_map.insert(map<string, int>::value_type("MERGE", -323));
	reserved_map.insert(map<string, int>::value_type("MESSAGE_TEXT", -324));
	reserved_map.insert(map<string, int>::value_type("MICROSECOND", -325));
	reserved_map.insert(map<string, int>::value_type("MIDDLEINT", -326));
	reserved_map.insert(map<string, int>::value_type("MIGRATE", -327));
	reserved_map.insert(map<string, int>::value_type("MINUTE", -328));
	reserved_map.insert(map<string, int>::value_type("MINUTE_MICROSECOND", -329));
	reserved_map.insert(map<string, int>::value_type("MINUTE_SECOND", -330));
	reserved_map.insert(map<string, int>::value_type("MIN_ROWS", -331));
	reserved_map.insert(map<string, int>::value_type("MOD", -332));
	reserved_map.insert(map<string, int>::value_type("MODE", -333));
	reserved_map.insert(map<string, int>::value_type("MODIFIES", -334));
	reserved_map.insert(map<string, int>::value_type("MODIFY", -335));
	reserved_map.insert(map<string, int>::value_type("MONTH", -336));
	reserved_map.insert(map<string, int>::value_type("MULTILINESTRING", -337));
	reserved_map.insert(map<string, int>::value_type("MULTIPOINT", -338));
	reserved_map.insert(map<string, int>::value_type("MULTIPOLYGON", -339));
	reserved_map.insert(map<string, int>::value_type("MUTEX", -340));
	reserved_map.insert(map<string, int>::value_type("MYSQL_ERRNO", -341));
	reserved_map.insert(map<string, int>::value_type("NAME", -342));
	reserved_map.insert(map<string, int>::value_type("NAMES", -343));
	reserved_map.insert(map<string, int>::value_type("NATIONAL", -344));
	reserved_map.insert(map<string, int>::value_type("NATURAL", -345));
	reserved_map.insert(map<string, int>::value_type("NCHAR", -346));
	reserved_map.insert(map<string, int>::value_type("NDB", -347));
	reserved_map.insert(map<string, int>::value_type("NDBCLUSTER", -348));
	reserved_map.insert(map<string, int>::value_type("NEVER", -349));
	reserved_map.insert(map<string, int>::value_type("NEW", -350));
	reserved_map.insert(map<string, int>::value_type("NEXT", -351));
	reserved_map.insert(map<string, int>::value_type("NO", -352));
	reserved_map.insert(map<string, int>::value_type("NODEGROUP", -353));
	reserved_map.insert(map<string, int>::value_type("NONBLOCKING", -354));
	reserved_map.insert(map<string, int>::value_type("NONE", -355));
	reserved_map.insert(map<string, int>::value_type("NOT", -356));
	reserved_map.insert(map<string, int>::value_type("NO_WAIT", -357));
	reserved_map.insert(map<string, int>::value_type("NO_WRITE_TO_BINLOG", -358));
	reserved_map.insert(map<string, int>::value_type("NULL", -359));
	reserved_map.insert(map<string, int>::value_type("NUMBER", -360));
	reserved_map.insert(map<string, int>::value_type("NUMERIC", -361));
	reserved_map.insert(map<string, int>::value_type("NVARCHAR", -362));
	reserved_map.insert(map<string, int>::value_type("OFFSET", -363));
	reserved_map.insert(map<string, int>::value_type("OLD_PASSWORD", -364));
	reserved_map.insert(map<string, int>::value_type("ON", -365));
	reserved_map.insert(map<string, int>::value_type("ONE", -366));
	reserved_map.insert(map<string, int>::value_type("ONLY", -367));
	reserved_map.insert(map<string, int>::value_type("OPEN", -368));
	reserved_map.insert(map<string, int>::value_type("OPTIMIZE", -369));
	reserved_map.insert(map<string, int>::value_type("OPTIMIZER_COSTS", -370));
	reserved_map.insert(map<string, int>::value_type("OPTION", -371));
	reserved_map.insert(map<string, int>::value_type("OPTIONALLY", -372));
	reserved_map.insert(map<string, int>::value_type("OPTIONS", -373));
	reserved_map.insert(map<string, int>::value_type("OR", -374));
	reserved_map.insert(map<string, int>::value_type("ORDER", -375));
	reserved_map.insert(map<string, int>::value_type("OUT", -376));
	reserved_map.insert(map<string, int>::value_type("OUTER", -377));
	reserved_map.insert(map<string, int>::value_type("OUTFILE", -378));
	reserved_map.insert(map<string, int>::value_type("OWNER", -379));
	reserved_map.insert(map<string, int>::value_type("PACK_KEYS", -380));
	reserved_map.insert(map<string, int>::value_type("PAGE", -381));
	reserved_map.insert(map<string, int>::value_type("PARSER", -382));
	reserved_map.insert(map<string, int>::value_type("PARSE_GCOL_EXPR", -383));
	reserved_map.insert(map<string, int>::value_type("PARTIAL", -384));
	reserved_map.insert(map<string, int>::value_type("PARTITION", -385));
	reserved_map.insert(map<string, int>::value_type("PARTITIONING", -386));
	reserved_map.insert(map<string, int>::value_type("PARTITIONS", -387));
	reserved_map.insert(map<string, int>::value_type("PASSWORD", -388));
	reserved_map.insert(map<string, int>::value_type("PHASE", -389));
	reserved_map.insert(map<string, int>::value_type("PLUGIN", -390));
	reserved_map.insert(map<string, int>::value_type("PLUGINS", -391));
	reserved_map.insert(map<string, int>::value_type("PLUGIN_DIR", -392));
	reserved_map.insert(map<string, int>::value_type("POINT", -393));
	reserved_map.insert(map<string, int>::value_type("POLYGON", -394));
	reserved_map.insert(map<string, int>::value_type("PORT", -395));
	reserved_map.insert(map<string, int>::value_type("PRECEDES", -396));
	reserved_map.insert(map<string, int>::value_type("PRECISION", -397));
	reserved_map.insert(map<string, int>::value_type("PREPARE", -398));
	reserved_map.insert(map<string, int>::value_type("PRESERVE", -399));
	reserved_map.insert(map<string, int>::value_type("PREV", -400));
	reserved_map.insert(map<string, int>::value_type("PRIMARY", -401));
	reserved_map.insert(map<string, int>::value_type("PRIVILEGES", -402));
	reserved_map.insert(map<string, int>::value_type("PROCEDURE", -403));
	reserved_map.insert(map<string, int>::value_type("PROCESSLIST", -404));
	reserved_map.insert(map<string, int>::value_type("PROFILE", -405));
	reserved_map.insert(map<string, int>::value_type("PROFILES", -406));
	reserved_map.insert(map<string, int>::value_type("PROXY", -407));
	reserved_map.insert(map<string, int>::value_type("PURGE", -408));
	reserved_map.insert(map<string, int>::value_type("QUARTER", -409));
	reserved_map.insert(map<string, int>::value_type("QUERY", -410));
	reserved_map.insert(map<string, int>::value_type("QUICK", -411));
	reserved_map.insert(map<string, int>::value_type("RANGE", -412));
	reserved_map.insert(map<string, int>::value_type("READ", -413));
	reserved_map.insert(map<string, int>::value_type("READS", -414));
	reserved_map.insert(map<string, int>::value_type("READ_ONLY", -415));
	reserved_map.insert(map<string, int>::value_type("READ_WRITE", -416));
	reserved_map.insert(map<string, int>::value_type("REAL", -417));
	reserved_map.insert(map<string, int>::value_type("REBUILD", -418));
	reserved_map.insert(map<string, int>::value_type("RECOVER", -419));
	reserved_map.insert(map<string, int>::value_type("REDOFILE", -420));
	reserved_map.insert(map<string, int>::value_type("REDO_BUFFER_SIZE", -421));
	reserved_map.insert(map<string, int>::value_type("REDUNDANT", -422));
	reserved_map.insert(map<string, int>::value_type("REFERENCES", -423));
	reserved_map.insert(map<string, int>::value_type("REGEXP", -424));
	reserved_map.insert(map<string, int>::value_type("RELAY", -425));
	reserved_map.insert(map<string, int>::value_type("RELAYLOG", -426));
	reserved_map.insert(map<string, int>::value_type("RELAY_LOG_FILE", -427));
	reserved_map.insert(map<string, int>::value_type("RELAY_LOG_POS", -428));
	reserved_map.insert(map<string, int>::value_type("RELAY_THREAD", -429));
	reserved_map.insert(map<string, int>::value_type("RELEASE", -430));
	reserved_map.insert(map<string, int>::value_type("RELOAD", -431));
	reserved_map.insert(map<string, int>::value_type("REMOVE", -432));
	reserved_map.insert(map<string, int>::value_type("RENAME", -433));
	reserved_map.insert(map<string, int>::value_type("REORGANIZE", -434));
	reserved_map.insert(map<string, int>::value_type("REPAIR", -435));
	reserved_map.insert(map<string, int>::value_type("REPEAT", -436));
	reserved_map.insert(map<string, int>::value_type("REPEATABLE", -437));
	reserved_map.insert(map<string, int>::value_type("REPLACE", -438));
	reserved_map.insert(map<string, int>::value_type("REPLICATE_DO_DB", -439));
	reserved_map.insert(map<string, int>::value_type("REPLICATE_DO_TABLE", -440));
	reserved_map.insert(map<string, int>::value_type("REPLICATE_IGNORE_DB", -441));
	reserved_map.insert(map<string, int>::value_type("REPLICATE_IGNORE_TABLE", -442));
	reserved_map.insert(map<string, int>::value_type("REPLICATE_REWRITE_DB", -443));
	reserved_map.insert(map<string, int>::value_type("REPLICATE_WILD_DO_TABLE", -444));
	reserved_map.insert(map<string, int>::value_type("REPLICATE_WILD_IGNORE_TABLE", -445));
	reserved_map.insert(map<string, int>::value_type("REPLICATION", -446));
	reserved_map.insert(map<string, int>::value_type("REQUIRE", -447));
	reserved_map.insert(map<string, int>::value_type("RESET", -448));
	reserved_map.insert(map<string, int>::value_type("RESIGNAL", -449));
	reserved_map.insert(map<string, int>::value_type("RESTORE", -450));
	reserved_map.insert(map<string, int>::value_type("RESTRICT", -451));
	reserved_map.insert(map<string, int>::value_type("RESUME", -452));
	reserved_map.insert(map<string, int>::value_type("RETURN", -453));
	reserved_map.insert(map<string, int>::value_type("RETURNED_SQLSTATE", -454));
	reserved_map.insert(map<string, int>::value_type("RETURNS", -455));
	reserved_map.insert(map<string, int>::value_type("REVERSE", -456));
	reserved_map.insert(map<string, int>::value_type("REVOKE", -457));
	reserved_map.insert(map<string, int>::value_type("RIGHT", -458));
	reserved_map.insert(map<string, int>::value_type("RLIKE", -459));
	reserved_map.insert(map<string, int>::value_type("ROLLBACK", -460));
	reserved_map.insert(map<string, int>::value_type("ROLLUP", -461));
	reserved_map.insert(map<string, int>::value_type("ROTATE", -462));
	reserved_map.insert(map<string, int>::value_type("ROUTINE", -463));
	reserved_map.insert(map<string, int>::value_type("ROW", -464));
	reserved_map.insert(map<string, int>::value_type("ROWS", -465));
	reserved_map.insert(map<string, int>::value_type("ROW_COUNT", -466));
	reserved_map.insert(map<string, int>::value_type("ROW_FORMAT", -467));
	reserved_map.insert(map<string, int>::value_type("RTREE", -468));
	reserved_map.insert(map<string, int>::value_type("SAVEPOINT", -469));
	reserved_map.insert(map<string, int>::value_type("SCHEDULE", -470));
	reserved_map.insert(map<string, int>::value_type("SCHEMA", -471));
	reserved_map.insert(map<string, int>::value_type("SCHEMAS", -472));
	reserved_map.insert(map<string, int>::value_type("SCHEMA_NAME", -473));
	reserved_map.insert(map<string, int>::value_type("SECOND", -474));
	reserved_map.insert(map<string, int>::value_type("SECOND_MICROSECOND", -475));
	reserved_map.insert(map<string, int>::value_type("SECURITY", -476));
	reserved_map.insert(map<string, int>::value_type("SELECT", -477));
	reserved_map.insert(map<string, int>::value_type("SENSITIVE", -478));
	reserved_map.insert(map<string, int>::value_type("SEPARATOR", -479));
	reserved_map.insert(map<string, int>::value_type("SERIAL", -480));
	reserved_map.insert(map<string, int>::value_type("SERIALIZABLE", -481));
	reserved_map.insert(map<string, int>::value_type("SERVER", -482));
	reserved_map.insert(map<string, int>::value_type("SESSION", -483));
	reserved_map.insert(map<string, int>::value_type("SET", -484));
	reserved_map.insert(map<string, int>::value_type("SHARE", -485));
	reserved_map.insert(map<string, int>::value_type("SHOW", -486));
	reserved_map.insert(map<string, int>::value_type("SHUTDOWN", -487));
	reserved_map.insert(map<string, int>::value_type("SIGNAL", -488));
	reserved_map.insert(map<string, int>::value_type("SIGNED", -489));
	reserved_map.insert(map<string, int>::value_type("SIMPLE", -490));
	reserved_map.insert(map<string, int>::value_type("SLAVE", -491));
	reserved_map.insert(map<string, int>::value_type("SLOW", -492));
	reserved_map.insert(map<string, int>::value_type("SMALLINT", -493));
	reserved_map.insert(map<string, int>::value_type("SNAPSHOT", -494));
	reserved_map.insert(map<string, int>::value_type("SOCKET", -495));
	reserved_map.insert(map<string, int>::value_type("SOME", -496));
	reserved_map.insert(map<string, int>::value_type("SONAME", -497));
	reserved_map.insert(map<string, int>::value_type("SOUNDS", -498));
	reserved_map.insert(map<string, int>::value_type("SOURCE", -499));
	reserved_map.insert(map<string, int>::value_type("SPATIAL", -500));
	reserved_map.insert(map<string, int>::value_type("SPECIFIC", -501));
	reserved_map.insert(map<string, int>::value_type("SQL", -502));
	reserved_map.insert(map<string, int>::value_type("SQLEXCEPTION", -503));
	reserved_map.insert(map<string, int>::value_type("SQLSTATE", -504));
	reserved_map.insert(map<string, int>::value_type("SQLWARNING", -505));
	reserved_map.insert(map<string, int>::value_type("SQL_AFTER_GTIDS", -506));
	reserved_map.insert(map<string, int>::value_type("SQL_AFTER_MTS_GAPS", -507));
	reserved_map.insert(map<string, int>::value_type("SQL_BEFORE_GTIDS", -508));
	reserved_map.insert(map<string, int>::value_type("SQL_BIG_RESULT", -509));
	reserved_map.insert(map<string, int>::value_type("SQL_BUFFER_RESULT", -510));
	reserved_map.insert(map<string, int>::value_type("SQL_CACHE", -511));
	reserved_map.insert(map<string, int>::value_type("SQL_CALC_FOUND_ROWS", -512));
	reserved_map.insert(map<string, int>::value_type("SQL_NO_CACHE", -513));
	reserved_map.insert(map<string, int>::value_type("SQL_SMALL_RESULT", -514));
	reserved_map.insert(map<string, int>::value_type("SQL_THREAD", -515));
	reserved_map.insert(map<string, int>::value_type("SQL_TSI_DAY", -516));
	reserved_map.insert(map<string, int>::value_type("SQL_TSI_FRAC_SECOND", -517));
	reserved_map.insert(map<string, int>::value_type("SQL_TSI_HOUR", -518));
	reserved_map.insert(map<string, int>::value_type("SQL_TSI_MINUTE", -519));
	reserved_map.insert(map<string, int>::value_type("SQL_TSI_MONTH", -520));
	reserved_map.insert(map<string, int>::value_type("SQL_TSI_QUARTER", -521));
	reserved_map.insert(map<string, int>::value_type("SQL_TSI_SECOND", -522));
	reserved_map.insert(map<string, int>::value_type("SQL_TSI_WEEK", -523));
	reserved_map.insert(map<string, int>::value_type("SQL_TSI_YEAR", -524));
	reserved_map.insert(map<string, int>::value_type("SSL", -525));
	reserved_map.insert(map<string, int>::value_type("STACKED", -526));
	reserved_map.insert(map<string, int>::value_type("START", -527));
	reserved_map.insert(map<string, int>::value_type("STARTING", -528));
	reserved_map.insert(map<string, int>::value_type("STARTS", -529));
	reserved_map.insert(map<string, int>::value_type("STATS_AUTO_RECALC", -530));
	reserved_map.insert(map<string, int>::value_type("STATS_PERSISTENT", -531));
	reserved_map.insert(map<string, int>::value_type("STATS_SAMPLE_PAGES", -532));
	reserved_map.insert(map<string, int>::value_type("STATUS", -533));
	reserved_map.insert(map<string, int>::value_type("STOP", -534));
	reserved_map.insert(map<string, int>::value_type("STORAGE", -535));
	reserved_map.insert(map<string, int>::value_type("STORED", -536));
	reserved_map.insert(map<string, int>::value_type("STRAIGHT_JOIN", -537));
	reserved_map.insert(map<string, int>::value_type("STRING", -538));
	reserved_map.insert(map<string, int>::value_type("STRIPED", -539));
	reserved_map.insert(map<string, int>::value_type("SUBCLASS_ORIGIN", -540));
	reserved_map.insert(map<string, int>::value_type("SUBJECT", -541));
	reserved_map.insert(map<string, int>::value_type("SUBPARTITION", -542));
	reserved_map.insert(map<string, int>::value_type("SUBPARTITIONS", -543));
	reserved_map.insert(map<string, int>::value_type("SUPER", -544));
	reserved_map.insert(map<string, int>::value_type("SUSPEND", -545));
	reserved_map.insert(map<string, int>::value_type("SWAPS", -546));
	reserved_map.insert(map<string, int>::value_type("SWITCHES", -547));
	reserved_map.insert(map<string, int>::value_type("TABLE", -548));
	reserved_map.insert(map<string, int>::value_type("TABLES", -549));
	reserved_map.insert(map<string, int>::value_type("TABLESPACE", -550));
	reserved_map.insert(map<string, int>::value_type("TABLE_CHECKSUM", -551));
	reserved_map.insert(map<string, int>::value_type("TABLE_NAME", -552));
	reserved_map.insert(map<string, int>::value_type("TEMPORARY", -553));
	reserved_map.insert(map<string, int>::value_type("TEMPTABLE", -554));
	reserved_map.insert(map<string, int>::value_type("TERMINATED", -555));
	reserved_map.insert(map<string, int>::value_type("TEXT", -556));
	reserved_map.insert(map<string, int>::value_type("THAN", -557));
	reserved_map.insert(map<string, int>::value_type("THEN", -558));
	reserved_map.insert(map<string, int>::value_type("TIME", -559));
	reserved_map.insert(map<string, int>::value_type("TIMESTAMP", -560));
	reserved_map.insert(map<string, int>::value_type("TIMESTAMPADD", -561));
	reserved_map.insert(map<string, int>::value_type("TIMESTAMPDIFF", -562));
	reserved_map.insert(map<string, int>::value_type("TINYBLOB", -563));
	reserved_map.insert(map<string, int>::value_type("TINYINT", -564));
	reserved_map.insert(map<string, int>::value_type("TINYTEXT", -565));
	reserved_map.insert(map<string, int>::value_type("TO", -566));
	reserved_map.insert(map<string, int>::value_type("TRAILING", -567));
	reserved_map.insert(map<string, int>::value_type("TRANSACTION", -568));
	reserved_map.insert(map<string, int>::value_type("TRIGGER", -569));
	reserved_map.insert(map<string, int>::value_type("TRIGGERS", -570));
	reserved_map.insert(map<string, int>::value_type("TRUE", -571));
	reserved_map.insert(map<string, int>::value_type("TRUNCATE", -572));
	reserved_map.insert(map<string, int>::value_type("TYPE", -573));
	reserved_map.insert(map<string, int>::value_type("TYPES", -574));
	reserved_map.insert(map<string, int>::value_type("UNCOMMITTED", -575));
	reserved_map.insert(map<string, int>::value_type("UNDEFINED", -576));
	reserved_map.insert(map<string, int>::value_type("UNDO", -577));
	reserved_map.insert(map<string, int>::value_type("UNDOFILE", -578));
	reserved_map.insert(map<string, int>::value_type("UNDO_BUFFER_SIZE", -579));
	reserved_map.insert(map<string, int>::value_type("UNICODE", -580));
	reserved_map.insert(map<string, int>::value_type("UNINSTALL", -581));
	reserved_map.insert(map<string, int>::value_type("UNION", -582));
	reserved_map.insert(map<string, int>::value_type("UNIQUE", -583));
	reserved_map.insert(map<string, int>::value_type("UNKNOWN", -584));
	reserved_map.insert(map<string, int>::value_type("UNLOCK", -585));
	reserved_map.insert(map<string, int>::value_type("UNSIGNED", -586));
	reserved_map.insert(map<string, int>::value_type("UNTIL", -587));
	reserved_map.insert(map<string, int>::value_type("UPDATE", -588));
	reserved_map.insert(map<string, int>::value_type("UPGRADE", -589));
	reserved_map.insert(map<string, int>::value_type("USAGE", -590));
	reserved_map.insert(map<string, int>::value_type("USE", -591));
	reserved_map.insert(map<string, int>::value_type("USER", -592));
	reserved_map.insert(map<string, int>::value_type("USER_RESOURCES", -593));
	reserved_map.insert(map<string, int>::value_type("USE_FRM", -594));
	reserved_map.insert(map<string, int>::value_type("USING", -595));
	reserved_map.insert(map<string, int>::value_type("UTC_DATE", -596));
	reserved_map.insert(map<string, int>::value_type("UTC_TIME", -597));
	reserved_map.insert(map<string, int>::value_type("UTC_TIMESTAMP", -598));
	reserved_map.insert(map<string, int>::value_type("VALIDATION", -599));
	reserved_map.insert(map<string, int>::value_type("VALUE", -600));
	reserved_map.insert(map<string, int>::value_type("VALUES", -601));
	reserved_map.insert(map<string, int>::value_type("VARBINARY", -602));
	reserved_map.insert(map<string, int>::value_type("VARCHAR", -603));
	reserved_map.insert(map<string, int>::value_type("VARCHARACTER", -604));
	reserved_map.insert(map<string, int>::value_type("VARIABLES", -605));
	reserved_map.insert(map<string, int>::value_type("VARYING", -606));
	reserved_map.insert(map<string, int>::value_type("VIEW", -607));
	reserved_map.insert(map<string, int>::value_type("VIRTUAL", -608));
	reserved_map.insert(map<string, int>::value_type("WAIT", -609));
	reserved_map.insert(map<string, int>::value_type("WARNINGS", -610));
	reserved_map.insert(map<string, int>::value_type("WEEK", -611));
	reserved_map.insert(map<string, int>::value_type("WEIGHT_STRING", -612));
	reserved_map.insert(map<string, int>::value_type("WHEN", -613));
	reserved_map.insert(map<string, int>::value_type("WHERE", -614));
	reserved_map.insert(map<string, int>::value_type("WHILE", -615));
	reserved_map.insert(map<string, int>::value_type("WITH", -616));
	reserved_map.insert(map<string, int>::value_type("WITHOUT", -617));
	reserved_map.insert(map<string, int>::value_type("WORK", -618));
	reserved_map.insert(map<string, int>::value_type("WRAPPER", -619));
	reserved_map.insert(map<string, int>::value_type("WRITE", -620));
	reserved_map.insert(map<string, int>::value_type("X509", -621));
	reserved_map.insert(map<string, int>::value_type("XA", -622));
	reserved_map.insert(map<string, int>::value_type("XID", -623));
	reserved_map.insert(map<string, int>::value_type("XML", -624));
	reserved_map.insert(map<string, int>::value_type("XOR", -625));
	reserved_map.insert(map<string, int>::value_type("YEAR", -626));
	reserved_map.insert(map<string, int>::value_type("YEAR_MONTH", -627));
	reserved_map.insert(map<string, int>::value_type("ZEROFILL", -628));
}





bool isidchar(int c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

void scroll_Char(int* ahead)
{
	int i = 0;
	for (; i < LookAheadLen - 1 ; ++i)
		ahead[i] = ahead[i + 1];

	if (ahead[LookAheadLen - 1] != EOF)
		ahead[LookAheadLen - 1] = getchar();
}
namespace scan_nsp
{
	class scan_error :public runtime_error
	{
	public:
		explicit scan_error(const string& s) :
			runtime_error(s) {}
	};

	class comment_incomplete_error :public scan_error
	{
	public:
		explicit comment_incomplete_error(const string& s) :
			scan_error(s) {}
	};

	class string_error :public scan_error
	{
	public:
		explicit string_error(const std::string& s):
			scan_error(s){}
	};

	token gettok()
	{
		if (scanner_status != blank)
		{
			fprintf(stderr, "scanner_status is not blank，but you simply call gettok() \n");
		}

		// handle EOF case
		if (LookAhead[0] == EOF)
		{
			token t;
			t.token_kind = eof;
			t.token_value = eof_value();
			return t;
		}

		// Skip any whitespace.
		while (isspace(LookAhead[0]))
		{
			scroll_Char(LookAhead);
		}
		// # style comment
		if (LookAhead[0] == '#')
		{
			scanner_status = comment;
			do
				scroll_Char(LookAhead);
			while (LookAhead[0] != EOF && LookAhead[0] != '\n' && LookAhead[0] != '\r');
			scanner_status = blank;
		}

		//-- style comment
		if (LookAhead[0] == '-'&& LookAhead[1] == '-')
		{
			scanner_status = comment;
			do
				scroll_Char(LookAhead);
			while (LookAhead[0] != EOF && LookAhead[0] != '\n' && LookAhead[0] != '\r');
			scanner_status = blank;
		}

		// /* */ style comment 注意 /*/ 不是合法的注释
		if (LookAhead[0] == '/'&& LookAhead[1] == '*')
		{
			scanner_status = comment;
			scroll_Char(LookAhead); scroll_Char(LookAhead);
			while (!(LookAhead[0] == '*' && LookAhead[1] == '/'))
			{
				scroll_Char(LookAhead);
				if (LookAhead[0] == EOF)
					throw scan_nsp::comment_incomplete_error(R"zjulab("/*" mis-matches "*/" )zjulab");
			}
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			scanner_status = blank;
		}

		// 字符串字面量 肥肠值得注意 \的转义
		// 相连的两个'' 或 "" 可合并为1个
		// 相连的两个字符串的粘连动作交给 parser 处理
		if (LookAhead[0] == '"' || LookAhead[0] == '\'')
		{
			scanner_status = literal_string;
			auto match_char = LookAhead[0];
			scroll_Char(LookAhead);
			string_literal.clear();
			while (1)
			{
				if (LookAhead[0] != match_char && LookAhead[0] != '\\')
				{
					string_literal += static_cast<char>(LookAhead[0]);
					scroll_Char(LookAhead);
					continue;
				}

				if (LookAhead[0] == '\\')
				{
					if (LookAhead[1] == '0')
					{
						string_literal += '\0';
						scroll_Char(LookAhead);
						scroll_Char(LookAhead);
						continue;
					}
					if (LookAhead[1] == '\'')
					{
						string_literal += '\'';
						scroll_Char(LookAhead);
						scroll_Char(LookAhead);
						continue;
					}
					if (LookAhead[1] == '"')
					{
						string_literal += '"';
						scroll_Char(LookAhead);
						scroll_Char(LookAhead);
						continue;
					}
					if (LookAhead[1] == 'b')
					{
						string_literal += '\b';
						scroll_Char(LookAhead);
						scroll_Char(LookAhead);
						continue;
					}
					if (LookAhead[1] == 'n')
					{
						string_literal += '\n';
						scroll_Char(LookAhead);
						scroll_Char(LookAhead);
						continue;
					}
					if (LookAhead[1] == 'r')
					{
						string_literal += '\r';
						scroll_Char(LookAhead);
						scroll_Char(LookAhead);
						continue;
					}
					if (LookAhead[1] == 't')
					{
						string_literal += '\t';
						scroll_Char(LookAhead);
						scroll_Char(LookAhead);
						continue;
					}

					if (LookAhead[1] == '\\')
					{
						string_literal += '\\';
						scroll_Char(LookAhead);
						scroll_Char(LookAhead);
						continue;
					}

					continue;
				}

				if (LookAhead[0] == match_char&&LookAhead[1] == match_char)
				{
					string_literal += (match_char == '"' ? '"' : '\'');
					scroll_Char(LookAhead);
					scroll_Char(LookAhead);
					continue;
				}

				if (LookAhead[0] == match_char&&isspace(LookAhead[1]))
				{
					auto t = token();
					t.token_kind = literal_string;
					t.token_value = string_value(string_literal);
					scroll_Char(LookAhead);
					scanner_status = blank;
					return t;
				}

				if (LookAhead[0] == EOF)
					throw scan_nsp::string_error((string{ "" }+(match_char == '"' ? '"' : '\'')) + " in string literal dismatches");
			}
		}

		// 特殊照顾一下 true  false 这两个看起来像关键字的字面量
		if (tolower(LookAhead[0]) == 't' && tolower(LookAhead[1]) == 'r' && \
			tolower(LookAhead[2]) == 'u' && tolower(LookAhead[3]) == 'e')
		{
			scanner_status = literal_int;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = int_value(1);
			scanner_status = blank;
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			return t;
		}

		if (tolower(LookAhead[0]) == 'f' && tolower(LookAhead[1]) == 'a' && \
			tolower(LookAhead[2]) == 'l' && tolower(LookAhead[3]) == 's' && tolower(LookAhead[4]) == 'e')
		{
			scanner_status = literal_int;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = int_value(0);
			scanner_status = blank;
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			return t;
		}

		// 只处理[0-9]和小数点. 正负号留给parser处理。
		// 只处理10进制，不处理 0x 0b 等字面量
		// 011视作十进制11而不是8进制
		// 数值字面量的几种模式
		// int就是 [0-9]+
		// 出现小数点. 或指数e 均为double
		// 以下为一些测试
		// 0256 2048
		// 0.36
		// .345
		// 5e6
		// .2e-2
		//23.6
		//3.25e5
		//.3e4
		//65e-8
		//3.
		//0
		//23.36e-9
		//6.e9


		if (isdigit(LookAhead[0]) || (LookAhead[0] == '.'&& isdigit(LookAhead[1])))
		{
			scanner_status = literal_int;
			string numeric_str;
			regex numeric_regex(R"zjulabregex((([0-9]*\.[0-9]+)|[0-9]+\.?)(e[+-]?[0-9]+)?)zjulabregex");
			numeric_str += static_cast<char>(LookAhead[0]);
			scroll_Char(LookAhead);
			while (regex_match(numeric_str + static_cast<char>(LookAhead[0]), numeric_regex))
			{
				numeric_str += static_cast<char>(LookAhead[0]);
				scroll_Char(LookAhead);
			}
			if ((numeric_str.find('e') == std::string::npos) && (numeric_str.find('.') == std::string::npos));
			else scanner_status = literal_double;

			if (scanner_status == literal_int)
			{
				token t;
				t.token_kind = int_literal;
				t.token_value = int_value(atoi(numeric_str.c_str()));
				scanner_status = blank;
				return t;
			}
			else
			{
				token t;
				t.token_kind = double_literal;
				t.token_value = double_value(atof(numeric_str.c_str()));
				scanner_status = blank;
				return t;
			}
		}

		// 运算符
		// 务必先判断长的运算符比如<=>
		// 后判断短的运算符比如<=
		if (LookAhead[0] == '<' && LookAhead[1] == '=' && LookAhead[2] == '>')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(lteqgt_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '<' && LookAhead[1] == '<')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(left_shift_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '>' && LookAhead[1] == '>')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(right_shift_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '>' && LookAhead[1] == '=')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(gteq_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '<' && LookAhead[1] == '=')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(lteq_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '<' && LookAhead[1] == '>')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(ltgt_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '!' && LookAhead[1] == '=')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(noteq_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '&' && LookAhead[1] == '&')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(andand_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '|' && LookAhead[1] == '|')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(oror_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == ':' && LookAhead[1] == '=')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(assign_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '!')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(not_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '-')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(minus_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '~')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(tilde_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '^')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(hat_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '*')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(mult_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '/')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(div_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '%')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(mod_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '+')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(plus_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '&')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(and_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '|')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(or_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '=')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(eq_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '>')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(gt_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '<')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(lt_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '#')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(number_sign_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '@')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(at_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '$')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(dollar_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == ',')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(comma_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '(')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(left_bracket_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == ')')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(right_bracket_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '[')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(left_square_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == ']')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(right_square_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '{')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(left_curly_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '}')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(right_curly_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '.')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(dot_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == ';')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(semicolon_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '?')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(qusetion_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		if (LookAhead[0] == '`')
		{
			scanner_status = symbol_mark;
			auto t = token();
			t.token_kind = scanner_status;
			t.token_value = reserved_value(backquote_mark);
			scanner_status = blank;
			scroll_Char(LookAhead);
			return t;
		}

		// 保留字与变量名
		IdentifierStr.clear();
		do
		{
			IdentifierStr += LookAhead[0];
			scroll_Char(LookAhead);
		} while (isidchar(LookAhead[0]));
		auto t = token();
		if (find(reserved_dict.cbegin(), reserved_dict.cend(), IdentifierStr) != reserved_dict.cend())
		{
			scanner_status = symbol_mark;
			t.token_kind = scanner_status;
			t.token_value = reserved_value(reserved_map[IdentifierStr]);
		}
		else
		{
			scanner_status = id;
			t.token_kind = scanner_status;
			t.token_value = id_value(IdentifierStr);
		}

		scanner_status = blank;
		return t;
	}
}




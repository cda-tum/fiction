// Benchmark "b12" written by ABC on Wed Sep  5 10:17:20 2018

module b12 ( clock, 
    START, K_3_, K_2_, K_1_, K_0_,
    NLOSS_REG, NL_REG_3_, NL_REG_2_, NL_REG_1_, NL_REG_0_, SPEAKER_REG  );
  input  clock;
  input  START, K_3_, K_2_, K_1_, K_0_;
  output NLOSS_REG, NL_REG_3_, NL_REG_2_, NL_REG_1_, NL_REG_0_, SPEAKER_REG;
  reg COUNT_REG_0_, MEMORY_REG_31__1_, MEMORY_REG_31__0_, MEMORY_REG_30__1_,
    MEMORY_REG_30__0_, MEMORY_REG_29__1_, MEMORY_REG_29__0_,
    MEMORY_REG_28__1_, MEMORY_REG_28__0_, MEMORY_REG_27__1_,
    MEMORY_REG_27__0_, MEMORY_REG_26__1_, MEMORY_REG_26__0_,
    MEMORY_REG_25__1_, MEMORY_REG_25__0_, MEMORY_REG_24__1_,
    MEMORY_REG_24__0_, MEMORY_REG_23__1_, MEMORY_REG_23__0_,
    MEMORY_REG_22__1_, MEMORY_REG_22__0_, MEMORY_REG_21__1_,
    MEMORY_REG_21__0_, MEMORY_REG_20__1_, MEMORY_REG_20__0_,
    MEMORY_REG_19__1_, MEMORY_REG_19__0_, MEMORY_REG_18__1_,
    MEMORY_REG_18__0_, MEMORY_REG_17__1_, MEMORY_REG_17__0_,
    MEMORY_REG_16__1_, MEMORY_REG_16__0_, MEMORY_REG_15__1_,
    MEMORY_REG_15__0_, MEMORY_REG_14__1_, MEMORY_REG_14__0_,
    MEMORY_REG_13__1_, MEMORY_REG_13__0_, MEMORY_REG_12__1_,
    MEMORY_REG_12__0_, MEMORY_REG_11__1_, MEMORY_REG_11__0_,
    MEMORY_REG_10__1_, MEMORY_REG_10__0_, MEMORY_REG_9__1_,
    MEMORY_REG_9__0_, MEMORY_REG_8__1_, MEMORY_REG_8__0_, MEMORY_REG_7__1_,
    MEMORY_REG_7__0_, MEMORY_REG_6__1_, MEMORY_REG_6__0_, MEMORY_REG_5__1_,
    MEMORY_REG_5__0_, MEMORY_REG_4__1_, MEMORY_REG_4__0_, MEMORY_REG_3__1_,
    MEMORY_REG_3__0_, MEMORY_REG_2__1_, MEMORY_REG_2__0_, MEMORY_REG_1__1_,
    MEMORY_REG_1__0_, MEMORY_REG_0__1_, MEMORY_REG_0__0_, NL_REG_3_,
    NL_REG_2_, NL_REG_1_, NL_REG_0_, SCAN_REG_4_, SCAN_REG_3_, SCAN_REG_2_,
    SCAN_REG_1_, SCAN_REG_0_, MAX_REG_4_, MAX_REG_3_, MAX_REG_2_,
    MAX_REG_1_, MAX_REG_0_, IND_REG_1_, IND_REG_0_, TIMEBASE_REG_5_,
    TIMEBASE_REG_4_, TIMEBASE_REG_3_, TIMEBASE_REG_2_, TIMEBASE_REG_1_,
    TIMEBASE_REG_0_, COUNT_REG2_5_, COUNT_REG2_4_, COUNT_REG2_3_,
    COUNT_REG2_2_, COUNT_REG2_1_, COUNT_REG2_0_, SOUND_REG_2_,
    SOUND_REG_1_, SOUND_REG_0_, ADDRESS_REG_4_, ADDRESS_REG_3_,
    ADDRESS_REG_2_, ADDRESS_REG_1_, ADDRESS_REG_0_, DATA_IN_REG_1_,
    DATA_IN_REG_0_, S_REG, PLAY_REG, NLOSS_REG, SPEAKER_REG, WR_REG,
    COUNTER_REG_2_, COUNTER_REG_1_, COUNTER_REG_0_, COUNT_REG_1_,
    NUM_REG_1_, NUM_REG_0_, DATA_OUT_REG_1_, DATA_OUT_REG_0_, GAMMA_REG_4_,
    GAMMA_REG_3_, GAMMA_REG_2_, GAMMA_REG_1_, GAMMA_REG_0_;
  wire n375_1, n376, n377, n378, n379, n380_1, n381, n382, n384, n385_1,
    n386, n388, n389, n390_1, n391, n392, n394, n395_1, n397, n398, n399,
    n400_1, n401, n402, n404, n405_1, n407, n408, n409, n410_1, n412, n413,
    n415_1, n416, n417, n418, n419, n421, n422, n424, n425_1, n426, n427,
    n428, n430_1, n431, n433, n434, n435_1, n436, n438, n439, n441, n442,
    n443, n444, n446, n447, n449, n450_1, n451, n452, n453, n454, n456,
    n457, n459, n460_1, n461, n462, n464, n465_1, n467, n468, n469, n470_1,
    n471, n472, n474, n475_1, n477, n478, n479, n480_1, n482, n483, n485_1,
    n486, n487, n488, n490_1, n491, n493, n494, n495_1, n496, n498, n499,
    n501, n502, n503, n504, n506, n507, n509, n510_1, n511, n512, n514,
    n515_1, n517, n518, n519, n520_1, n521, n522, n524, n525_1, n527, n528,
    n529, n530_1, n532, n533, n535_1, n536, n537, n538, n539, n540_1, n542,
    n543, n545_1, n546, n547, n548, n550, n551, n553_1, n554, n555, n556,
    n558_1, n559, n561, n562, n563_1, n564, n566, n567, n569, n570, n571,
    n572, n574, n575, n577_1, n578, n579, n580, n582, n583, n585, n586_1,
    n587, n588, n589, n590, n592, n593, n595, n596_1, n597, n598, n600,
    n601_1, n603, n604, n605, n606_1, n607, n609, n610, n612, n613, n614,
    n615, n617, n618, n620, n621, n622, n623, n625, n626, n628, n629, n630,
    n631, n633, n634, n636, n637, n638, n639, n641, n642, n644, n645, n646,
    n647, n649, n650, n652, n653, n654, n655, n656, n657, n658, n659, n660,
    n661, n662, n663, n664, n665, n666, n667, n668, n669, n670, n671, n672,
    n673, n674, n675, n676, n677, n678, n679, n680, n681, n682, n683, n684,
    n685, n686, n687, n688, n689, n690, n691, n692, n693, n694, n695, n696,
    n697, n698, n699, n700, n701, n702, n703, n704, n705, n706, n707, n708,
    n709, n710, n711, n712, n713, n714, n715, n716, n717, n718, n719, n720,
    n721, n722, n723, n724, n725, n726, n727, n728, n729, n730, n731, n732,
    n733, n734, n736, n737, n738, n739, n740, n741, n742, n744, n745, n746,
    n747, n748, n749, n750, n752, n753, n754, n755, n756, n757, n758, n760,
    n761, n762, n763, n764, n765, n766, n767, n768, n769, n770, n771, n772,
    n773, n774, n775, n776, n777, n778, n779, n780, n781, n783, n784, n785,
    n786, n787, n789, n790, n791, n792, n793, n795, n796, n797, n798, n799,
    n801, n802, n804, n805, n806, n807, n808, n809, n810, n811, n812, n813,
    n814, n815, n816, n817, n818, n819, n820, n821, n822, n823, n824, n825,
    n826, n827, n828, n829, n830, n831, n833, n834, n835, n836, n837, n839,
    n840, n841, n842, n843, n845, n846, n847, n848, n849, n851, n852, n854,
    n855, n856, n857, n858, n859, n860, n862, n863, n864, n865, n867, n868,
    n869, n870, n871, n872, n873, n874, n875, n876, n877, n878, n879, n880,
    n881, n882, n883, n884, n885, n886, n887, n888, n890, n891, n892, n893,
    n894, n896, n897, n898, n899, n901, n902, n903, n904, n906, n907, n908,
    n909, n911, n912, n913, n915, n916, n917, n918, n919, n920, n921, n922,
    n923, n924, n925, n926, n927, n928, n929, n930, n931, n932, n933, n934,
    n935, n936, n937, n938, n939, n940, n941, n942, n943, n944, n945, n946,
    n947, n948, n949, n950, n951, n952, n953, n954, n955, n956, n957, n958,
    n959, n960, n961, n962, n963, n964, n965, n966, n967, n968, n969, n970,
    n971, n972, n973, n974, n975, n976, n977, n978, n980, n981, n982, n983,
    n984, n985, n986, n987, n988, n989, n990, n991, n992, n993, n994, n995,
    n996, n998, n999, n1000, n1001, n1002, n1003, n1004, n1005, n1006,
    n1007, n1008, n1009, n1011, n1012, n1013, n1014, n1016, n1017, n1018,
    n1019, n1021, n1022, n1023, n1024, n1025, n1026, n1028, n1029, n1030,
    n1031, n1032, n1033, n1034, n1035, n1036, n1037, n1038, n1039, n1040,
    n1041, n1042, n1043, n1045, n1046, n1047, n1048, n1049, n1051, n1052,
    n1053, n1054, n1055, n1056, n1058, n1059, n1060, n1061, n1062, n1063,
    n1064, n1065, n1066, n1067, n1068, n1069, n1071, n1072, n1073, n1074,
    n1076, n1077, n1078, n1079, n1081, n1082, n1083, n1084, n1086, n1087,
    n1088, n1089, n1091, n1092, n1094, n1095, n1097, n1098, n1099, n1100,
    n1101, n1102, n1103, n1104, n1105, n1106, n1107, n1108, n1109, n1110,
    n1111, n1112, n1113, n1114, n1115, n1116, n1117, n1119, n1120, n1121,
    n1122, n1123, n1124, n1125, n1126, n1127, n1128, n1129, n1130, n1131,
    n1132, n1133, n1134, n1135, n1136, n1137, n1139, n1140, n1141, n1142,
    n1143, n1144, n1145, n1146, n1147, n1148, n1149, n1150, n1151, n1152,
    n1153, n1154, n1155, n1157, n1158, n1160, n1161, n1163, n1164, n1165,
    n1166, n1168, n1169, n1170, n1173, n1174, n1176, n1177, n1178, n1179,
    n1180, n1181, n1182, n1183, n1184, n1185, n1186, n1187, n1188, n1189,
    n1190, n1191, n1192, n1193, n1194, n1195, n1196, n1197, n1198, n1199,
    n1200, n1201, n1202, n1203, n1204, n1205, n1206, n1207, n1208, n1209,
    n1210, n1211, n1212, n1213, n1214, n1215, n1216, n1217, n1218, n1219,
    n1220, n1221, n1222, n1223, n1224, n1225, n1226, n1227, n1228, n1229,
    n1230, n1231, n1232, n1233, n1234, n1235, n1236, n1237, n1239, n1240,
    n1241, n1242, n1243, n1244, n1245, n1246, n1247, n1248, n1249, n1250,
    n1251, n1252, n1253, n1254, n1255, n1256, n1257, n1258, n1259, n1260,
    n1261, n1262, n1263, n1264, n1265, n1266, n1267, n1268, n1269, n1270,
    n1271, n1272, n1273, n1274, n1275, n1276, n1277, n1278, n1279, n1280,
    n1281, n1282, n1283, n1284, n1285, n1286, n1287, n1288, n1289, n1290,
    n1291, n1292, n1293, n1294, n1295, n1296, n1297, n1298, n1299, n1300,
    n1302, n1303, n1304, n1306, n1307, n1308, n1309, n1310, n1311, n1312,
    n1313, n1314, n1315, n1316, n1317, n1318, n1319, n1320, n1322, n1323,
    n1324, n1325, n1326, n1327, n1328, n1329, n1330, n1331, n1332, n1333,
    n1334, n1335, n1337, n1338, n1339, n1340, n1341, n1342, n1343, n1344,
    n1345, n1346, n1347, n1348, n1349, n1350, n1351, n1353, n1354, n1355,
    n1356, n1357, n1358, n1359, n1360, n1361, n1362, n1363, n1364, n1365,
    n1366, n1367, n1368, n1369, n1370, n1371, n1372, n1373, n1374, n1375,
    n24, n29, n34, n39, n44, n49, n54, n59, n64, n69, n74, n79, n84, n89,
    n94, n99, n104, n109, n114, n119, n124, n129, n134, n139, n144, n149,
    n154, n159, n164, n169, n174, n179, n184, n189, n194, n199, n204, n209,
    n214, n219, n224, n229, n234, n239, n244, n249, n254, n259, n264, n269,
    n274, n279, n284, n289, n294, n299, n304, n309, n314, n319, n324, n329,
    n334, n339, n344, n349, n353, n357, n361, n365, n370, n375, n380, n385,
    n390, n395, n400, n405, n410, n415, n420, n425, n430, n435, n440, n445,
    n450, n455, n460, n465, n470, n475, n480, n485, n490, n495, n500, n505,
    n510, n515, n520, n525, n530, n535, n540, n545, n549, n553, n558, n563,
    n568, n573, n577, n581, n586, n591, n596, n601, n606, n611, n616;
  assign n375_1 = DATA_IN_REG_1_ & WR_REG;
  assign n376 = ADDRESS_REG_4_ & ADDRESS_REG_3_;
  assign n377 = ADDRESS_REG_1_ & n376;
  assign n378 = ADDRESS_REG_2_ & ADDRESS_REG_0_;
  assign n379 = n377 & n378;
  assign n380_1 = n375_1 & n379;
  assign n381 = WR_REG & n379;
  assign n382 = MEMORY_REG_31__1_ & ~n381;
  assign n29 = n380_1 | n382;
  assign n384 = DATA_IN_REG_0_ & WR_REG;
  assign n385_1 = n379 & n384;
  assign n386 = MEMORY_REG_31__0_ & ~n381;
  assign n34 = n385_1 | n386;
  assign n388 = ADDRESS_REG_2_ & ~ADDRESS_REG_0_;
  assign n389 = n377 & n388;
  assign n390_1 = n375_1 & n389;
  assign n391 = WR_REG & n389;
  assign n392 = MEMORY_REG_30__1_ & ~n391;
  assign n39 = n390_1 | n392;
  assign n394 = n384 & n389;
  assign n395_1 = MEMORY_REG_30__0_ & ~n391;
  assign n44 = n394 | n395_1;
  assign n397 = ADDRESS_REG_4_ & ~ADDRESS_REG_1_;
  assign n398 = ADDRESS_REG_3_ & n397;
  assign n399 = n378 & n398;
  assign n400_1 = n375_1 & n399;
  assign n401 = WR_REG & n399;
  assign n402 = MEMORY_REG_29__1_ & ~n401;
  assign n49 = n400_1 | n402;
  assign n404 = n384 & n399;
  assign n405_1 = MEMORY_REG_29__0_ & ~n401;
  assign n54 = n404 | n405_1;
  assign n407 = n388 & n398;
  assign n408 = n375_1 & n407;
  assign n409 = WR_REG & n407;
  assign n410_1 = MEMORY_REG_28__1_ & ~n409;
  assign n59 = n408 | n410_1;
  assign n412 = n384 & n407;
  assign n413 = MEMORY_REG_28__0_ & ~n409;
  assign n64 = n412 | n413;
  assign n415_1 = ~ADDRESS_REG_2_ & ADDRESS_REG_0_;
  assign n416 = n377 & n415_1;
  assign n417 = n375_1 & n416;
  assign n418 = WR_REG & n416;
  assign n419 = MEMORY_REG_27__1_ & ~n418;
  assign n69 = n417 | n419;
  assign n421 = n384 & n416;
  assign n422 = MEMORY_REG_27__0_ & ~n418;
  assign n74 = n421 | n422;
  assign n424 = ~ADDRESS_REG_2_ & ~ADDRESS_REG_0_;
  assign n425_1 = n377 & n424;
  assign n426 = n375_1 & n425_1;
  assign n427 = WR_REG & n425_1;
  assign n428 = MEMORY_REG_26__1_ & ~n427;
  assign n79 = n426 | n428;
  assign n430_1 = n384 & n425_1;
  assign n431 = MEMORY_REG_26__0_ & ~n427;
  assign n84 = n430_1 | n431;
  assign n433 = n398 & n415_1;
  assign n434 = n375_1 & n433;
  assign n435_1 = WR_REG & n433;
  assign n436 = MEMORY_REG_25__1_ & ~n435_1;
  assign n89 = n434 | n436;
  assign n438 = n384 & n433;
  assign n439 = MEMORY_REG_25__0_ & ~n435_1;
  assign n94 = n438 | n439;
  assign n441 = n398 & n424;
  assign n442 = n375_1 & n441;
  assign n443 = WR_REG & n441;
  assign n444 = MEMORY_REG_24__1_ & ~n443;
  assign n99 = n442 | n444;
  assign n446 = n384 & n441;
  assign n447 = MEMORY_REG_24__0_ & ~n443;
  assign n104 = n446 | n447;
  assign n449 = ADDRESS_REG_4_ & ~ADDRESS_REG_3_;
  assign n450_1 = ADDRESS_REG_1_ & n449;
  assign n451 = n378 & n450_1;
  assign n452 = n375_1 & n451;
  assign n453 = WR_REG & n451;
  assign n454 = MEMORY_REG_23__1_ & ~n453;
  assign n109 = n452 | n454;
  assign n456 = n384 & n451;
  assign n457 = MEMORY_REG_23__0_ & ~n453;
  assign n114 = n456 | n457;
  assign n459 = n388 & n450_1;
  assign n460_1 = n375_1 & n459;
  assign n461 = WR_REG & n459;
  assign n462 = MEMORY_REG_22__1_ & ~n461;
  assign n119 = n460_1 | n462;
  assign n464 = n384 & n459;
  assign n465_1 = MEMORY_REG_22__0_ & ~n461;
  assign n124 = n464 | n465_1;
  assign n467 = ~ADDRESS_REG_3_ & ~ADDRESS_REG_1_;
  assign n468 = ADDRESS_REG_4_ & n467;
  assign n469 = n378 & n468;
  assign n470_1 = n375_1 & n469;
  assign n471 = WR_REG & n469;
  assign n472 = MEMORY_REG_21__1_ & ~n471;
  assign n129 = n470_1 | n472;
  assign n474 = n384 & n469;
  assign n475_1 = MEMORY_REG_21__0_ & ~n471;
  assign n134 = n474 | n475_1;
  assign n477 = n388 & n468;
  assign n478 = n375_1 & n477;
  assign n479 = WR_REG & n477;
  assign n480_1 = MEMORY_REG_20__1_ & ~n479;
  assign n139 = n478 | n480_1;
  assign n482 = n384 & n477;
  assign n483 = MEMORY_REG_20__0_ & ~n479;
  assign n144 = n482 | n483;
  assign n485_1 = n415_1 & n450_1;
  assign n486 = n375_1 & n485_1;
  assign n487 = WR_REG & n485_1;
  assign n488 = MEMORY_REG_19__1_ & ~n487;
  assign n149 = n486 | n488;
  assign n490_1 = n384 & n485_1;
  assign n491 = MEMORY_REG_19__0_ & ~n487;
  assign n154 = n490_1 | n491;
  assign n493 = n424 & n450_1;
  assign n494 = n375_1 & n493;
  assign n495_1 = WR_REG & n493;
  assign n496 = MEMORY_REG_18__1_ & ~n495_1;
  assign n159 = n494 | n496;
  assign n498 = n384 & n493;
  assign n499 = MEMORY_REG_18__0_ & ~n495_1;
  assign n164 = n498 | n499;
  assign n501 = n415_1 & n468;
  assign n502 = n375_1 & n501;
  assign n503 = WR_REG & n501;
  assign n504 = MEMORY_REG_17__1_ & ~n503;
  assign n169 = n502 | n504;
  assign n506 = n384 & n501;
  assign n507 = MEMORY_REG_17__0_ & ~n503;
  assign n174 = n506 | n507;
  assign n509 = n424 & n468;
  assign n510_1 = n375_1 & n509;
  assign n511 = WR_REG & n509;
  assign n512 = MEMORY_REG_16__1_ & ~n511;
  assign n179 = n510_1 | n512;
  assign n514 = n384 & n509;
  assign n515_1 = MEMORY_REG_16__0_ & ~n511;
  assign n184 = n514 | n515_1;
  assign n517 = ~ADDRESS_REG_4_ & ADDRESS_REG_3_;
  assign n518 = ADDRESS_REG_1_ & n517;
  assign n519 = n378 & n518;
  assign n520_1 = n375_1 & n519;
  assign n521 = WR_REG & n519;
  assign n522 = MEMORY_REG_15__1_ & ~n521;
  assign n189 = n520_1 | n522;
  assign n524 = n384 & n519;
  assign n525_1 = MEMORY_REG_15__0_ & ~n521;
  assign n194 = n524 | n525_1;
  assign n527 = n388 & n518;
  assign n528 = n375_1 & n527;
  assign n529 = WR_REG & n527;
  assign n530_1 = MEMORY_REG_14__1_ & ~n529;
  assign n199 = n528 | n530_1;
  assign n532 = n384 & n527;
  assign n533 = MEMORY_REG_14__0_ & ~n529;
  assign n204 = n532 | n533;
  assign n535_1 = ~ADDRESS_REG_4_ & ~ADDRESS_REG_1_;
  assign n536 = ADDRESS_REG_3_ & n535_1;
  assign n537 = n378 & n536;
  assign n538 = n375_1 & n537;
  assign n539 = WR_REG & n537;
  assign n540_1 = MEMORY_REG_13__1_ & ~n539;
  assign n209 = n538 | n540_1;
  assign n542 = n384 & n537;
  assign n543 = MEMORY_REG_13__0_ & ~n539;
  assign n214 = n542 | n543;
  assign n545_1 = n388 & n536;
  assign n546 = n375_1 & n545_1;
  assign n547 = WR_REG & n545_1;
  assign n548 = MEMORY_REG_12__1_ & ~n547;
  assign n219 = n546 | n548;
  assign n550 = n384 & n545_1;
  assign n551 = MEMORY_REG_12__0_ & ~n547;
  assign n224 = n550 | n551;
  assign n553_1 = n415_1 & n518;
  assign n554 = n375_1 & n553_1;
  assign n555 = WR_REG & n553_1;
  assign n556 = MEMORY_REG_11__1_ & ~n555;
  assign n229 = n554 | n556;
  assign n558_1 = n384 & n553_1;
  assign n559 = MEMORY_REG_11__0_ & ~n555;
  assign n234 = n558_1 | n559;
  assign n561 = n424 & n518;
  assign n562 = n375_1 & n561;
  assign n563_1 = WR_REG & n561;
  assign n564 = MEMORY_REG_10__1_ & ~n563_1;
  assign n239 = n562 | n564;
  assign n566 = n384 & n561;
  assign n567 = MEMORY_REG_10__0_ & ~n563_1;
  assign n244 = n566 | n567;
  assign n569 = n415_1 & n536;
  assign n570 = n375_1 & n569;
  assign n571 = WR_REG & n569;
  assign n572 = MEMORY_REG_9__1_ & ~n571;
  assign n249 = n570 | n572;
  assign n574 = n384 & n569;
  assign n575 = MEMORY_REG_9__0_ & ~n571;
  assign n254 = n574 | n575;
  assign n577_1 = n424 & n536;
  assign n578 = n375_1 & n577_1;
  assign n579 = WR_REG & n577_1;
  assign n580 = MEMORY_REG_8__1_ & ~n579;
  assign n259 = n578 | n580;
  assign n582 = n384 & n577_1;
  assign n583 = MEMORY_REG_8__0_ & ~n579;
  assign n264 = n582 | n583;
  assign n585 = ~ADDRESS_REG_4_ & ~ADDRESS_REG_3_;
  assign n586_1 = ADDRESS_REG_1_ & n585;
  assign n587 = n378 & n586_1;
  assign n588 = n375_1 & n587;
  assign n589 = WR_REG & n587;
  assign n590 = MEMORY_REG_7__1_ & ~n589;
  assign n269 = n588 | n590;
  assign n592 = n384 & n587;
  assign n593 = MEMORY_REG_7__0_ & ~n589;
  assign n274 = n592 | n593;
  assign n595 = n388 & n586_1;
  assign n596_1 = n375_1 & n595;
  assign n597 = WR_REG & n595;
  assign n598 = MEMORY_REG_6__1_ & ~n597;
  assign n279 = n596_1 | n598;
  assign n600 = n384 & n595;
  assign n601_1 = MEMORY_REG_6__0_ & ~n597;
  assign n284 = n600 | n601_1;
  assign n603 = ~ADDRESS_REG_4_ & n467;
  assign n604 = n378 & n603;
  assign n605 = n375_1 & n604;
  assign n606_1 = WR_REG & n604;
  assign n607 = MEMORY_REG_5__1_ & ~n606_1;
  assign n289 = n605 | n607;
  assign n609 = n384 & n604;
  assign n610 = MEMORY_REG_5__0_ & ~n606_1;
  assign n294 = n609 | n610;
  assign n612 = n388 & n603;
  assign n613 = n375_1 & n612;
  assign n614 = WR_REG & n612;
  assign n615 = MEMORY_REG_4__1_ & ~n614;
  assign n299 = n613 | n615;
  assign n617 = n384 & n612;
  assign n618 = MEMORY_REG_4__0_ & ~n614;
  assign n304 = n617 | n618;
  assign n620 = n415_1 & n586_1;
  assign n621 = n375_1 & n620;
  assign n622 = WR_REG & n620;
  assign n623 = MEMORY_REG_3__1_ & ~n622;
  assign n309 = n621 | n623;
  assign n625 = n384 & n620;
  assign n626 = MEMORY_REG_3__0_ & ~n622;
  assign n314 = n625 | n626;
  assign n628 = n424 & n586_1;
  assign n629 = n375_1 & n628;
  assign n630 = WR_REG & n628;
  assign n631 = MEMORY_REG_2__1_ & ~n630;
  assign n319 = n629 | n631;
  assign n633 = n384 & n628;
  assign n634 = MEMORY_REG_2__0_ & ~n630;
  assign n324 = n633 | n634;
  assign n636 = n415_1 & n603;
  assign n637 = n375_1 & n636;
  assign n638 = WR_REG & n636;
  assign n639 = MEMORY_REG_1__1_ & ~n638;
  assign n329 = n637 | n639;
  assign n641 = n384 & n636;
  assign n642 = MEMORY_REG_1__0_ & ~n638;
  assign n334 = n641 | n642;
  assign n644 = n424 & n603;
  assign n645 = n375_1 & n644;
  assign n646 = WR_REG & n644;
  assign n647 = MEMORY_REG_0__1_ & ~n646;
  assign n339 = n645 | n647;
  assign n649 = n384 & n644;
  assign n650 = MEMORY_REG_0__0_ & ~n646;
  assign n344 = n649 | n650;
  assign n652 = ~START & GAMMA_REG_3_;
  assign n653 = ~START & GAMMA_REG_1_;
  assign n654 = ~START & ~GAMMA_REG_0_;
  assign n655 = n653 & n654;
  assign n656 = ~START & GAMMA_REG_2_;
  assign n657 = ~START & GAMMA_REG_4_;
  assign n658 = ~n656 & ~n657;
  assign n659 = ~n655 & ~n658;
  assign n660 = n652 & ~n659;
  assign n661 = ~n652 & ~n657;
  assign n662 = ~n653 & n656;
  assign n663 = n661 & n662;
  assign n664 = ~n653 & ~n654;
  assign n665 = ~n652 & n664;
  assign n666 = ~n652 & ~n656;
  assign n667 = ~n665 & ~n666;
  assign n668 = n657 & ~n667;
  assign n669 = n653 & ~n654;
  assign n670 = n657 & n669;
  assign n671 = n656 & n670;
  assign n672 = ~n663 & ~n668;
  assign n673 = ~n671 & n672;
  assign n674 = n652 & n658;
  assign n675 = n669 & n674;
  assign n676 = n666 & n670;
  assign n677 = ~COUNT_REG2_5_ & ~COUNT_REG2_3_;
  assign n678 = ~COUNT_REG2_4_ & n677;
  assign n679 = ~COUNT_REG2_2_ & ~COUNT_REG2_1_;
  assign n680 = ~COUNT_REG2_0_ & n679;
  assign n681 = n678 & n680;
  assign n682 = n652 & ~n664;
  assign n683 = ~SCAN_REG_2_ & MAX_REG_2_;
  assign n684 = SCAN_REG_2_ & ~MAX_REG_2_;
  assign n685 = ~SCAN_REG_1_ & MAX_REG_1_;
  assign n686 = SCAN_REG_1_ & ~MAX_REG_1_;
  assign n687 = ~SCAN_REG_0_ & MAX_REG_0_;
  assign n688 = SCAN_REG_0_ & ~MAX_REG_0_;
  assign n689 = ~n685 & ~n686;
  assign n690 = ~n687 & n689;
  assign n691 = ~n688 & n690;
  assign n692 = ~SCAN_REG_4_ & MAX_REG_4_;
  assign n693 = SCAN_REG_4_ & ~MAX_REG_4_;
  assign n694 = ~SCAN_REG_3_ & MAX_REG_3_;
  assign n695 = SCAN_REG_3_ & ~MAX_REG_3_;
  assign n696 = ~n692 & ~n693;
  assign n697 = ~n694 & n696;
  assign n698 = ~n695 & n697;
  assign n699 = ~n683 & ~n684;
  assign n700 = n691 & n699;
  assign n701 = n698 & n700;
  assign n702 = n665 & n701;
  assign n703 = ~n655 & ~n682;
  assign n704 = ~n702 & n703;
  assign n705 = n656 & n681;
  assign n706 = ~n704 & n705;
  assign n707 = ~n657 & n665;
  assign n708 = n656 & ~n664;
  assign n709 = ~n652 & ~n708;
  assign n710 = n657 & ~n709;
  assign n711 = n681 & n710;
  assign n712 = ~n707 & ~n711;
  assign n713 = n652 & n656;
  assign n714 = n654 & n713;
  assign n715 = n653 & ~n657;
  assign n716 = n714 & n715;
  assign n717 = ~n675 & ~n676;
  assign n718 = ~n706 & n717;
  assign n719 = n712 & n718;
  assign n720 = ~n716 & n719;
  assign n721 = ~n660 & n673;
  assign n722 = ~n720 & n721;
  assign n723 = NL_REG_3_ & ~n722;
  assign n724 = ~n673 & ~n720;
  assign n725 = DATA_OUT_REG_1_ & DATA_OUT_REG_0_;
  assign n726 = n724 & n725;
  assign n727 = n660 & ~n720;
  assign n728 = IND_REG_1_ & IND_REG_0_;
  assign n729 = n727 & n728;
  assign n730 = n652 & n657;
  assign n731 = n654 & n730;
  assign n732 = ~n720 & n731;
  assign n733 = ~n723 & ~n726;
  assign n734 = ~n729 & n733;
  assign n349 = n732 | ~n734;
  assign n736 = NL_REG_2_ & ~n722;
  assign n737 = IND_REG_1_ & ~IND_REG_0_;
  assign n738 = n727 & n737;
  assign n739 = DATA_OUT_REG_1_ & ~DATA_OUT_REG_0_;
  assign n740 = n724 & n739;
  assign n741 = ~n736 & ~n738;
  assign n742 = ~n740 & n741;
  assign n353 = n732 | ~n742;
  assign n744 = NL_REG_1_ & ~n722;
  assign n745 = ~DATA_OUT_REG_1_ & DATA_OUT_REG_0_;
  assign n746 = n724 & n745;
  assign n747 = ~IND_REG_1_ & IND_REG_0_;
  assign n748 = n727 & n747;
  assign n749 = ~n744 & ~n746;
  assign n750 = ~n748 & n749;
  assign n357 = n732 | ~n750;
  assign n752 = NL_REG_0_ & ~n722;
  assign n753 = ~IND_REG_1_ & ~IND_REG_0_;
  assign n754 = n727 & n753;
  assign n755 = ~DATA_OUT_REG_1_ & ~DATA_OUT_REG_0_;
  assign n756 = n724 & n755;
  assign n757 = ~n752 & ~n754;
  assign n758 = ~n756 & n757;
  assign n361 = n732 | ~n758;
  assign n760 = n654 & n661;
  assign n761 = n653 & ~n656;
  assign n762 = n760 & n761;
  assign n763 = ~n654 & n661;
  assign n764 = n653 & n656;
  assign n765 = n763 & n764;
  assign n766 = n681 & n765;
  assign n767 = n664 & n713;
  assign n768 = n681 & ~n701;
  assign n769 = n767 & n768;
  assign n770 = ~n762 & ~n766;
  assign n771 = ~n769 & n770;
  assign n772 = SCAN_REG_4_ & n771;
  assign n773 = SCAN_REG_1_ & SCAN_REG_0_;
  assign n774 = SCAN_REG_2_ & n773;
  assign n775 = SCAN_REG_3_ & n774;
  assign n776 = ~SCAN_REG_4_ & n775;
  assign n777 = SCAN_REG_4_ & ~n775;
  assign n778 = ~n776 & ~n777;
  assign n779 = n656 & ~n701;
  assign n780 = ~n771 & n779;
  assign n781 = ~n778 & n780;
  assign n365 = n772 | n781;
  assign n783 = SCAN_REG_3_ & n771;
  assign n784 = ~SCAN_REG_3_ & n774;
  assign n785 = SCAN_REG_3_ & ~n774;
  assign n786 = ~n784 & ~n785;
  assign n787 = n780 & ~n786;
  assign n370 = n783 | n787;
  assign n789 = SCAN_REG_2_ & n771;
  assign n790 = ~SCAN_REG_2_ & n773;
  assign n791 = SCAN_REG_2_ & ~n773;
  assign n792 = ~n790 & ~n791;
  assign n793 = n780 & ~n792;
  assign n375 = n789 | n793;
  assign n795 = SCAN_REG_1_ & n771;
  assign n796 = ~SCAN_REG_1_ & SCAN_REG_0_;
  assign n797 = SCAN_REG_1_ & ~SCAN_REG_0_;
  assign n798 = ~n796 & ~n797;
  assign n799 = n780 & ~n798;
  assign n380 = n795 | n799;
  assign n801 = SCAN_REG_0_ & n771;
  assign n802 = ~SCAN_REG_0_ & n780;
  assign n385 = n801 | n802;
  assign n804 = MAX_REG_1_ & MAX_REG_0_;
  assign n805 = MAX_REG_2_ & n804;
  assign n806 = MAX_REG_4_ & n805;
  assign n807 = MAX_REG_3_ & n806;
  assign n808 = n681 & n701;
  assign n809 = n767 & ~n807;
  assign n810 = n808 & n809;
  assign n811 = n658 & n665;
  assign n812 = ~n810 & ~n811;
  assign n813 = n652 & ~n656;
  assign n814 = n655 & n813;
  assign n815 = n657 & n664;
  assign n816 = n656 & n815;
  assign n817 = ~n701 & n816;
  assign n818 = ~n814 & ~n817;
  assign n819 = ~n653 & n654;
  assign n820 = n657 & n666;
  assign n821 = n819 & n820;
  assign n822 = n818 & ~n821;
  assign n823 = n681 & ~n822;
  assign n824 = n812 & ~n823;
  assign n825 = MAX_REG_4_ & n824;
  assign n826 = MAX_REG_3_ & n805;
  assign n827 = ~MAX_REG_4_ & n826;
  assign n828 = MAX_REG_4_ & ~n826;
  assign n829 = ~n827 & ~n828;
  assign n830 = n656 & ~n824;
  assign n831 = ~n829 & n830;
  assign n390 = n825 | n831;
  assign n833 = MAX_REG_3_ & n824;
  assign n834 = ~MAX_REG_3_ & n805;
  assign n835 = MAX_REG_3_ & ~n805;
  assign n836 = ~n834 & ~n835;
  assign n837 = n830 & ~n836;
  assign n395 = n833 | n837;
  assign n839 = MAX_REG_2_ & n824;
  assign n840 = ~MAX_REG_2_ & n804;
  assign n841 = MAX_REG_2_ & ~n804;
  assign n842 = ~n840 & ~n841;
  assign n843 = n830 & ~n842;
  assign n400 = n839 | n843;
  assign n845 = MAX_REG_1_ & n824;
  assign n846 = ~MAX_REG_1_ & MAX_REG_0_;
  assign n847 = MAX_REG_1_ & ~MAX_REG_0_;
  assign n848 = ~n846 & ~n847;
  assign n849 = n830 & ~n848;
  assign n405 = n845 | n849;
  assign n851 = MAX_REG_0_ & n824;
  assign n852 = ~MAX_REG_0_ & n830;
  assign n410 = n851 | n852;
  assign n854 = ~K_1_ & ~K_0_;
  assign n855 = ~K_3_ & ~K_2_;
  assign n856 = n854 & n855;
  assign n857 = ~n681 & ~n856;
  assign n858 = n814 & n857;
  assign n859 = n854 & n858;
  assign n860 = IND_REG_1_ & ~n858;
  assign n415 = n859 | n860;
  assign n862 = K_2_ & ~K_1_;
  assign n863 = ~K_0_ & ~n862;
  assign n864 = n858 & n863;
  assign n865 = IND_REG_0_ & ~n858;
  assign n420 = n864 | n865;
  assign n867 = TIMEBASE_REG_0_ & n681;
  assign n868 = ~COUNT_REG2_0_ & ~n867;
  assign n869 = TIMEBASE_REG_1_ & n681;
  assign n870 = ~COUNT_REG2_1_ & ~n869;
  assign n871 = n868 & n870;
  assign n872 = TIMEBASE_REG_2_ & n681;
  assign n873 = ~COUNT_REG2_2_ & ~n872;
  assign n874 = n871 & n873;
  assign n875 = TIMEBASE_REG_3_ & n681;
  assign n876 = ~COUNT_REG2_3_ & ~n875;
  assign n877 = n874 & n876;
  assign n878 = TIMEBASE_REG_4_ & n681;
  assign n879 = ~COUNT_REG2_4_ & ~n878;
  assign n880 = n877 & n879;
  assign n881 = TIMEBASE_REG_5_ & n681;
  assign n882 = ~COUNT_REG2_5_ & ~n881;
  assign n883 = n880 & n882;
  assign n884 = ~n880 & ~n882;
  assign n885 = ~n883 & ~n884;
  assign n886 = n652 & n885;
  assign n887 = ~n812 & ~n886;
  assign n888 = TIMEBASE_REG_5_ & n812;
  assign n425 = n887 | n888;
  assign n890 = TIMEBASE_REG_4_ & n812;
  assign n891 = ~n877 & ~n879;
  assign n892 = ~n880 & ~n891;
  assign n893 = n652 & ~n812;
  assign n894 = ~n892 & n893;
  assign n430 = n890 | n894;
  assign n896 = TIMEBASE_REG_3_ & n812;
  assign n897 = ~n874 & ~n876;
  assign n898 = ~n877 & ~n897;
  assign n899 = n893 & ~n898;
  assign n435 = n896 | n899;
  assign n901 = TIMEBASE_REG_2_ & n812;
  assign n902 = ~n871 & ~n873;
  assign n903 = ~n874 & ~n902;
  assign n904 = n893 & ~n903;
  assign n440 = n901 | n904;
  assign n906 = TIMEBASE_REG_1_ & n812;
  assign n907 = ~n868 & ~n870;
  assign n908 = ~n871 & ~n907;
  assign n909 = n893 & ~n908;
  assign n445 = n906 | n909;
  assign n911 = n652 & ~n868;
  assign n912 = ~n812 & ~n911;
  assign n913 = TIMEBASE_REG_0_ & n812;
  assign n450 = n912 | n913;
  assign n915 = n653 & n666;
  assign n916 = ~n653 & n661;
  assign n917 = n656 & n819;
  assign n918 = n681 & n917;
  assign n919 = n658 & ~n856;
  assign n920 = n653 & n713;
  assign n921 = ~n760 & ~n920;
  assign n922 = n681 & ~n921;
  assign n923 = ~n915 & ~n916;
  assign n924 = ~n918 & n923;
  assign n925 = ~n919 & n924;
  assign n926 = ~n922 & n925;
  assign n927 = TIMEBASE_REG_5_ & ~n926;
  assign n928 = ~n764 & ~n819;
  assign n929 = ~n681 & ~n928;
  assign n930 = n658 & n856;
  assign n931 = n652 & n664;
  assign n932 = ~n730 & ~n815;
  assign n933 = ~n931 & n932;
  assign n934 = ~n652 & n654;
  assign n935 = n933 & ~n934;
  assign n936 = ~n681 & ~n935;
  assign n937 = ~n929 & ~n930;
  assign n938 = ~n936 & n937;
  assign n939 = ~n885 & ~n938;
  assign n940 = ~n927 & ~n939;
  assign n941 = ~n654 & n920;
  assign n942 = ~n657 & n941;
  assign n943 = n653 & n760;
  assign n944 = ~n653 & n763;
  assign n945 = ~n652 & ~n664;
  assign n946 = n657 & n945;
  assign n947 = n669 & ~n681;
  assign n948 = ~n657 & n947;
  assign n949 = ~n943 & ~n944;
  assign n950 = ~n946 & n949;
  assign n951 = ~n948 & n950;
  assign n952 = n656 & ~n951;
  assign n953 = n808 & n816;
  assign n954 = ~n767 & ~n816;
  assign n955 = ~n681 & ~n954;
  assign n956 = ~n953 & ~n955;
  assign n957 = ~n656 & n657;
  assign n958 = ~n653 & n957;
  assign n959 = ~n657 & n819;
  assign n960 = n654 & ~n656;
  assign n961 = ~n681 & n960;
  assign n962 = ~n958 & ~n959;
  assign n963 = ~n961 & n962;
  assign n964 = n652 & ~n963;
  assign n965 = n819 & n957;
  assign n966 = n767 & n807;
  assign n967 = n808 & n966;
  assign n968 = ~n964 & ~n965;
  assign n969 = ~n676 & n968;
  assign n970 = ~n967 & n969;
  assign n971 = ~n942 & ~n952;
  assign n972 = n956 & n971;
  assign n973 = n970 & n972;
  assign n974 = ~n940 & ~n973;
  assign n975 = COUNT_REG2_5_ & n973;
  assign n976 = ~n653 & n658;
  assign n977 = ~n973 & n976;
  assign n978 = ~n974 & ~n975;
  assign n455 = n977 | ~n978;
  assign n980 = n653 & n658;
  assign n981 = n856 & n980;
  assign n982 = ~n681 & n708;
  assign n983 = ~n981 & ~n982;
  assign n984 = ~n936 & n983;
  assign n985 = ~n973 & ~n984;
  assign n986 = ~n892 & n985;
  assign n987 = ~n663 & ~n915;
  assign n988 = n653 & ~n856;
  assign n989 = n658 & n988;
  assign n990 = n987 & ~n989;
  assign n991 = ~n922 & n990;
  assign n992 = ~n918 & n991;
  assign n993 = ~n973 & ~n992;
  assign n994 = TIMEBASE_REG_4_ & n993;
  assign n995 = COUNT_REG2_4_ & n973;
  assign n996 = ~n986 & ~n994;
  assign n460 = n995 | ~n996;
  assign n998 = TIMEBASE_REG_3_ & ~n991;
  assign n999 = n656 & n657;
  assign n1000 = n653 & n999;
  assign n1001 = TIMEBASE_REG_3_ & n656;
  assign n1002 = ~n1000 & ~n1001;
  assign n1003 = n933 & n1002;
  assign n1004 = n681 & ~n1003;
  assign n1005 = ~n898 & ~n984;
  assign n1006 = ~n998 & ~n1004;
  assign n1007 = ~n1005 & n1006;
  assign n1008 = ~n973 & ~n1007;
  assign n1009 = COUNT_REG2_3_ & n973;
  assign n465 = n1008 | n1009;
  assign n1011 = ~n903 & n985;
  assign n1012 = TIMEBASE_REG_2_ & n993;
  assign n1013 = COUNT_REG2_2_ & n973;
  assign n1014 = ~n1011 & ~n1012;
  assign n470 = n1013 | ~n1014;
  assign n1016 = ~n908 & n985;
  assign n1017 = TIMEBASE_REG_1_ & n993;
  assign n1018 = COUNT_REG2_1_ & n973;
  assign n1019 = ~n1016 & ~n1017;
  assign n475 = n1018 | ~n1019;
  assign n1021 = TIMEBASE_REG_0_ & ~n926;
  assign n1022 = n868 & ~n938;
  assign n1023 = ~n1021 & ~n1022;
  assign n1024 = ~n973 & ~n1023;
  assign n1025 = COUNT_REG2_0_ & n973;
  assign n1026 = ~n1024 & ~n1025;
  assign n480 = n977 | ~n1026;
  assign n1028 = n657 & ~n666;
  assign n1029 = ~n713 & ~n1028;
  assign n1030 = ~n654 & n730;
  assign n1031 = ~n670 & ~n1030;
  assign n1032 = n681 & ~n1031;
  assign n1033 = n656 & n707;
  assign n1034 = n656 & n664;
  assign n1035 = n807 & n1034;
  assign n1036 = n808 & n1035;
  assign n1037 = ~n858 & ~n1036;
  assign n1038 = ~n676 & n1037;
  assign n1039 = ~n953 & n1038;
  assign n1040 = ~n1032 & ~n1033;
  assign n1041 = n1039 & n1040;
  assign n1042 = ~n1029 & ~n1041;
  assign n1043 = SOUND_REG_2_ & n1041;
  assign n485 = n1042 | n1043;
  assign n1045 = DATA_OUT_REG_1_ & ~n987;
  assign n1046 = n658 & n854;
  assign n1047 = ~n1045 & ~n1046;
  assign n1048 = ~n1041 & ~n1047;
  assign n1049 = SOUND_REG_1_ & n1041;
  assign n490 = n1048 | n1049;
  assign n1051 = DATA_OUT_REG_0_ & ~n652;
  assign n1052 = n658 & n863;
  assign n1053 = ~n999 & ~n1051;
  assign n1054 = ~n1052 & n1053;
  assign n1055 = ~n1041 & ~n1054;
  assign n1056 = SOUND_REG_0_ & n1041;
  assign n495 = n1055 | n1056;
  assign n1058 = ~n652 & ~n669;
  assign n1059 = n674 & n819;
  assign n1060 = ~n715 & ~n815;
  assign n1061 = n666 & ~n1060;
  assign n1062 = ~n1059 & ~n1061;
  assign n1063 = ~n1058 & ~n1062;
  assign n1064 = SCAN_REG_4_ & n1063;
  assign n1065 = ~n655 & ~n815;
  assign n1066 = ~n1062 & ~n1065;
  assign n1067 = MAX_REG_4_ & n1066;
  assign n1068 = ADDRESS_REG_4_ & n1062;
  assign n1069 = ~n1064 & ~n1067;
  assign n500 = n1068 | ~n1069;
  assign n1071 = SCAN_REG_3_ & n1063;
  assign n1072 = MAX_REG_3_ & n1066;
  assign n1073 = ADDRESS_REG_3_ & n1062;
  assign n1074 = ~n1071 & ~n1072;
  assign n505 = n1073 | ~n1074;
  assign n1076 = SCAN_REG_2_ & n1063;
  assign n1077 = MAX_REG_2_ & n1066;
  assign n1078 = ADDRESS_REG_2_ & n1062;
  assign n1079 = ~n1076 & ~n1077;
  assign n510 = n1078 | ~n1079;
  assign n1081 = SCAN_REG_1_ & n1063;
  assign n1082 = MAX_REG_1_ & n1066;
  assign n1083 = ADDRESS_REG_1_ & n1062;
  assign n1084 = ~n1081 & ~n1082;
  assign n515 = n1083 | ~n1084;
  assign n1086 = SCAN_REG_0_ & n1063;
  assign n1087 = MAX_REG_0_ & n1066;
  assign n1088 = ADDRESS_REG_0_ & n1062;
  assign n1089 = ~n1086 & ~n1087;
  assign n520 = n1088 | ~n1089;
  assign n1091 = NUM_REG_1_ & n762;
  assign n1092 = DATA_IN_REG_1_ & ~n762;
  assign n525 = n1091 | n1092;
  assign n1094 = NUM_REG_0_ & n762;
  assign n1095 = DATA_IN_REG_0_ & ~n762;
  assign n530 = n1094 | n1095;
  assign n1097 = SOUND_REG_2_ & SOUND_REG_0_;
  assign n1098 = ~SOUND_REG_0_ & COUNTER_REG_0_;
  assign n1099 = ~n1097 & ~n1098;
  assign n1100 = ~SOUND_REG_1_ & n1099;
  assign n1101 = COUNTER_REG_1_ & ~n1100;
  assign n1102 = SOUND_REG_1_ & ~SOUND_REG_0_;
  assign n1103 = COUNTER_REG_0_ & n1102;
  assign n1104 = ~n1101 & ~n1103;
  assign n1105 = SOUND_REG_2_ & ~SOUND_REG_0_;
  assign n1106 = ~SOUND_REG_1_ & ~n1105;
  assign n1107 = ~COUNTER_REG_2_ & ~n1106;
  assign n1108 = ~n1104 & ~n1107;
  assign n1109 = ~SOUND_REG_1_ & COUNTER_REG_2_;
  assign n1110 = ~n1105 & n1109;
  assign n1111 = ~n1108 & ~n1110;
  assign n1112 = SOUND_REG_2_ & SOUND_REG_1_;
  assign n1113 = ~n1111 & ~n1112;
  assign n1114 = ~S_REG & PLAY_REG;
  assign n1115 = n1113 & n1114;
  assign n1116 = PLAY_REG & n1113;
  assign n1117 = S_REG & ~n1116;
  assign n535 = n1115 | n1117;
  assign n1119 = ~n657 & n814;
  assign n1120 = ~n656 & ~n730;
  assign n1121 = ~n999 & ~n1120;
  assign n1122 = n664 & n1121;
  assign n1123 = ~n654 & n999;
  assign n1124 = ~n670 & ~n1123;
  assign n1125 = ~n652 & ~n1124;
  assign n1126 = ~n1119 & ~n1122;
  assign n1127 = ~n1125 & n1126;
  assign n1128 = ~n653 & n714;
  assign n1129 = ~n652 & n656;
  assign n1130 = n655 & n1129;
  assign n1131 = ~n941 & ~n1128;
  assign n1132 = ~n1130 & n1131;
  assign n1133 = n681 & ~n1132;
  assign n1134 = n712 & ~n1133;
  assign n1135 = n1039 & n1134;
  assign n1136 = ~n1127 & ~n1135;
  assign n1137 = PLAY_REG & n1135;
  assign n540 = n1136 | n1137;
  assign n1139 = n681 & n814;
  assign n1140 = K_1_ & ~n745;
  assign n1141 = ~K_0_ & n1140;
  assign n1142 = K_0_ & ~n755;
  assign n1143 = K_2_ & n739;
  assign n1144 = K_3_ & ~n725;
  assign n1145 = ~K_2_ & ~n1144;
  assign n1146 = ~n1143 & ~n1145;
  assign n1147 = n854 & n1146;
  assign n1148 = ~n1141 & ~n1142;
  assign n1149 = ~n1147 & n1148;
  assign n1150 = ~n681 & n814;
  assign n1151 = ~n1149 & n1150;
  assign n1152 = ~n811 & ~n1139;
  assign n1153 = ~n1151 & n1152;
  assign n1154 = n652 & ~n1153;
  assign n1155 = NLOSS_REG & n1153;
  assign n545 = n1154 | n1155;
  assign n1157 = PLAY_REG & SPEAKER_REG;
  assign n1158 = ~n1113 & n1157;
  assign n549 = n1115 | n1158;
  assign n1160 = ~n656 & n763;
  assign n1161 = WR_REG & ~n1160;
  assign n553 = n762 | n1161;
  assign n1163 = PLAY_REG & ~n1112;
  assign n1164 = n1111 & n1163;
  assign n1165 = COUNTER_REG_1_ & COUNTER_REG_0_;
  assign n1166 = ~COUNTER_REG_2_ & ~n1165;
  assign n558 = n1164 & ~n1166;
  assign n1168 = ~COUNTER_REG_1_ & COUNTER_REG_0_;
  assign n1169 = COUNTER_REG_1_ & ~COUNTER_REG_0_;
  assign n1170 = ~n1168 & ~n1169;
  assign n563 = n1164 & ~n1170;
  assign n568 = ~COUNTER_REG_0_ & n1164;
  assign n1173 = COUNT_REG_0_ & ~COUNT_REG_1_;
  assign n1174 = ~COUNT_REG_0_ & COUNT_REG_1_;
  assign n573 = n1173 | n1174;
  assign n1176 = MEMORY_REG_31__1_ & n379;
  assign n1177 = MEMORY_REG_30__1_ & n389;
  assign n1178 = MEMORY_REG_29__1_ & n399;
  assign n1179 = MEMORY_REG_28__1_ & n407;
  assign n1180 = MEMORY_REG_27__1_ & n416;
  assign n1181 = MEMORY_REG_26__1_ & n425_1;
  assign n1182 = MEMORY_REG_25__1_ & n433;
  assign n1183 = MEMORY_REG_24__1_ & n441;
  assign n1184 = ~n1180 & ~n1181;
  assign n1185 = ~n1182 & n1184;
  assign n1186 = ~n1183 & n1185;
  assign n1187 = ~n1176 & ~n1177;
  assign n1188 = ~n1178 & n1187;
  assign n1189 = ~n1179 & n1188;
  assign n1190 = n1186 & n1189;
  assign n1191 = MEMORY_REG_23__1_ & n451;
  assign n1192 = MEMORY_REG_22__1_ & n459;
  assign n1193 = MEMORY_REG_21__1_ & n469;
  assign n1194 = MEMORY_REG_20__1_ & n477;
  assign n1195 = MEMORY_REG_19__1_ & n485_1;
  assign n1196 = MEMORY_REG_18__1_ & n493;
  assign n1197 = MEMORY_REG_17__1_ & n501;
  assign n1198 = MEMORY_REG_16__1_ & n509;
  assign n1199 = ~n1195 & ~n1196;
  assign n1200 = ~n1197 & n1199;
  assign n1201 = ~n1198 & n1200;
  assign n1202 = ~n1191 & ~n1192;
  assign n1203 = ~n1193 & n1202;
  assign n1204 = ~n1194 & n1203;
  assign n1205 = n1201 & n1204;
  assign n1206 = MEMORY_REG_15__1_ & n519;
  assign n1207 = MEMORY_REG_14__1_ & n527;
  assign n1208 = MEMORY_REG_13__1_ & n537;
  assign n1209 = MEMORY_REG_12__1_ & n545_1;
  assign n1210 = MEMORY_REG_11__1_ & n553_1;
  assign n1211 = MEMORY_REG_10__1_ & n561;
  assign n1212 = MEMORY_REG_9__1_ & n569;
  assign n1213 = MEMORY_REG_8__1_ & n577_1;
  assign n1214 = ~n1210 & ~n1211;
  assign n1215 = ~n1212 & n1214;
  assign n1216 = ~n1213 & n1215;
  assign n1217 = ~n1206 & ~n1207;
  assign n1218 = ~n1208 & n1217;
  assign n1219 = ~n1209 & n1218;
  assign n1220 = n1216 & n1219;
  assign n1221 = MEMORY_REG_7__1_ & n587;
  assign n1222 = MEMORY_REG_6__1_ & n595;
  assign n1223 = MEMORY_REG_5__1_ & n604;
  assign n1224 = MEMORY_REG_4__1_ & n612;
  assign n1225 = MEMORY_REG_3__1_ & n620;
  assign n1226 = MEMORY_REG_2__1_ & n628;
  assign n1227 = MEMORY_REG_1__1_ & n636;
  assign n1228 = MEMORY_REG_0__1_ & n644;
  assign n1229 = ~n1225 & ~n1226;
  assign n1230 = ~n1227 & n1229;
  assign n1231 = ~n1228 & n1230;
  assign n1232 = ~n1221 & ~n1222;
  assign n1233 = ~n1223 & n1232;
  assign n1234 = ~n1224 & n1233;
  assign n1235 = n1231 & n1234;
  assign n1236 = n1190 & n1205;
  assign n1237 = n1220 & n1236;
  assign n586 = ~n1235 | ~n1237;
  assign n1239 = MEMORY_REG_31__0_ & n379;
  assign n1240 = MEMORY_REG_30__0_ & n389;
  assign n1241 = MEMORY_REG_29__0_ & n399;
  assign n1242 = MEMORY_REG_28__0_ & n407;
  assign n1243 = MEMORY_REG_27__0_ & n416;
  assign n1244 = MEMORY_REG_26__0_ & n425_1;
  assign n1245 = MEMORY_REG_25__0_ & n433;
  assign n1246 = MEMORY_REG_24__0_ & n441;
  assign n1247 = ~n1243 & ~n1244;
  assign n1248 = ~n1245 & n1247;
  assign n1249 = ~n1246 & n1248;
  assign n1250 = ~n1239 & ~n1240;
  assign n1251 = ~n1241 & n1250;
  assign n1252 = ~n1242 & n1251;
  assign n1253 = n1249 & n1252;
  assign n1254 = MEMORY_REG_23__0_ & n451;
  assign n1255 = MEMORY_REG_22__0_ & n459;
  assign n1256 = MEMORY_REG_21__0_ & n469;
  assign n1257 = MEMORY_REG_20__0_ & n477;
  assign n1258 = MEMORY_REG_19__0_ & n485_1;
  assign n1259 = MEMORY_REG_18__0_ & n493;
  assign n1260 = MEMORY_REG_17__0_ & n501;
  assign n1261 = MEMORY_REG_16__0_ & n509;
  assign n1262 = ~n1258 & ~n1259;
  assign n1263 = ~n1260 & n1262;
  assign n1264 = ~n1261 & n1263;
  assign n1265 = ~n1254 & ~n1255;
  assign n1266 = ~n1256 & n1265;
  assign n1267 = ~n1257 & n1266;
  assign n1268 = n1264 & n1267;
  assign n1269 = MEMORY_REG_15__0_ & n519;
  assign n1270 = MEMORY_REG_14__0_ & n527;
  assign n1271 = MEMORY_REG_13__0_ & n537;
  assign n1272 = MEMORY_REG_12__0_ & n545_1;
  assign n1273 = MEMORY_REG_11__0_ & n553_1;
  assign n1274 = MEMORY_REG_10__0_ & n561;
  assign n1275 = MEMORY_REG_9__0_ & n569;
  assign n1276 = MEMORY_REG_8__0_ & n577_1;
  assign n1277 = ~n1273 & ~n1274;
  assign n1278 = ~n1275 & n1277;
  assign n1279 = ~n1276 & n1278;
  assign n1280 = ~n1269 & ~n1270;
  assign n1281 = ~n1271 & n1280;
  assign n1282 = ~n1272 & n1281;
  assign n1283 = n1279 & n1282;
  assign n1284 = MEMORY_REG_7__0_ & n587;
  assign n1285 = MEMORY_REG_6__0_ & n595;
  assign n1286 = MEMORY_REG_5__0_ & n604;
  assign n1287 = MEMORY_REG_4__0_ & n612;
  assign n1288 = MEMORY_REG_3__0_ & n620;
  assign n1289 = MEMORY_REG_2__0_ & n628;
  assign n1290 = MEMORY_REG_1__0_ & n636;
  assign n1291 = MEMORY_REG_0__0_ & n644;
  assign n1292 = ~n1288 & ~n1289;
  assign n1293 = ~n1290 & n1292;
  assign n1294 = ~n1291 & n1293;
  assign n1295 = ~n1284 & ~n1285;
  assign n1296 = ~n1286 & n1295;
  assign n1297 = ~n1287 & n1296;
  assign n1298 = n1294 & n1297;
  assign n1299 = n1253 & n1268;
  assign n1300 = n1283 & n1299;
  assign n591 = ~n1298 | ~n1300;
  assign n1302 = ~n814 & ~n941;
  assign n1303 = n681 & ~n1302;
  assign n1304 = ~n967 & ~n1303;
  assign n596 = n657 | ~n1304;
  assign n1306 = n765 & n808;
  assign n1307 = ~n664 & ~n819;
  assign n1308 = ~n656 & ~n1307;
  assign n1309 = ~n656 & n669;
  assign n1310 = ~n1308 & ~n1309;
  assign n1311 = n652 & ~n1310;
  assign n1312 = n655 & n713;
  assign n1313 = ~n730 & ~n769;
  assign n1314 = ~n1312 & n1313;
  assign n1315 = ~n1128 & n1314;
  assign n1316 = ~n767 & n1302;
  assign n1317 = ~n681 & ~n1316;
  assign n1318 = ~n1306 & ~n1311;
  assign n1319 = n1315 & n1318;
  assign n1320 = ~n1317 & n1319;
  assign n601 = n966 | ~n1320;
  assign n1322 = ~n765 & ~n941;
  assign n1323 = ~n681 & ~n1322;
  assign n1324 = ~n1312 & ~n1323;
  assign n1325 = n662 & n760;
  assign n1326 = n655 & n656;
  assign n1327 = n819 & n999;
  assign n1328 = ~n1326 & ~n1327;
  assign n1329 = ~n1128 & n1328;
  assign n1330 = ~n1309 & n1329;
  assign n1331 = ~n671 & n1330;
  assign n1332 = ~n1033 & ~n1325;
  assign n1333 = ~n1151 & n1332;
  assign n1334 = n1331 & n1333;
  assign n1335 = n956 & n1324;
  assign n606 = ~n1334 | ~n1335;
  assign n1337 = n658 & n664;
  assign n1338 = ~n656 & n665;
  assign n1339 = ~n671 & ~n1326;
  assign n1340 = ~n810 & n1339;
  assign n1341 = ~n707 & n1340;
  assign n1342 = ~n953 & n1341;
  assign n1343 = n655 & n666;
  assign n1344 = n655 & n957;
  assign n1345 = ~n701 & n765;
  assign n1346 = ~n1343 & ~n1344;
  assign n1347 = ~n1345 & n1346;
  assign n1348 = n1324 & n1347;
  assign n1349 = ~n1337 & ~n1338;
  assign n1350 = ~n1150 & n1349;
  assign n1351 = n1342 & n1350;
  assign n611 = ~n1348 | ~n1351;
  assign n1353 = K_1_ & ~K_0_;
  assign n1354 = DATA_OUT_REG_0_ & ~n1353;
  assign n1355 = ~K_0_ & ~DATA_OUT_REG_0_;
  assign n1356 = ~n1354 & ~n1355;
  assign n1357 = ~DATA_OUT_REG_1_ & ~n1356;
  assign n1358 = K_3_ & ~K_2_;
  assign n1359 = DATA_OUT_REG_0_ & ~n1358;
  assign n1360 = ~K_2_ & ~DATA_OUT_REG_0_;
  assign n1361 = ~n1359 & ~n1360;
  assign n1362 = n854 & n1361;
  assign n1363 = DATA_OUT_REG_1_ & ~n1362;
  assign n1364 = ~n1357 & ~n1363;
  assign n1365 = n814 & n1364;
  assign n1366 = n954 & ~n1365;
  assign n1367 = ~n671 & n1366;
  assign n1368 = ~n1030 & n1367;
  assign n1369 = ~n681 & ~n1368;
  assign n1370 = ~n965 & n1329;
  assign n1371 = n818 & n1370;
  assign n1372 = n681 & ~n1371;
  assign n1373 = ~n1059 & ~n1325;
  assign n1374 = n1348 & n1373;
  assign n1375 = ~n1369 & n1374;
  assign n616 = n1372 | ~n1375;
  assign n24 = ~COUNT_REG_0_;
  assign n577 = n573;
  assign n581 = n24;
  always @ (posedge clock) begin
    COUNT_REG_0_ <= n24;
    MEMORY_REG_31__1_ <= n29;
    MEMORY_REG_31__0_ <= n34;
    MEMORY_REG_30__1_ <= n39;
    MEMORY_REG_30__0_ <= n44;
    MEMORY_REG_29__1_ <= n49;
    MEMORY_REG_29__0_ <= n54;
    MEMORY_REG_28__1_ <= n59;
    MEMORY_REG_28__0_ <= n64;
    MEMORY_REG_27__1_ <= n69;
    MEMORY_REG_27__0_ <= n74;
    MEMORY_REG_26__1_ <= n79;
    MEMORY_REG_26__0_ <= n84;
    MEMORY_REG_25__1_ <= n89;
    MEMORY_REG_25__0_ <= n94;
    MEMORY_REG_24__1_ <= n99;
    MEMORY_REG_24__0_ <= n104;
    MEMORY_REG_23__1_ <= n109;
    MEMORY_REG_23__0_ <= n114;
    MEMORY_REG_22__1_ <= n119;
    MEMORY_REG_22__0_ <= n124;
    MEMORY_REG_21__1_ <= n129;
    MEMORY_REG_21__0_ <= n134;
    MEMORY_REG_20__1_ <= n139;
    MEMORY_REG_20__0_ <= n144;
    MEMORY_REG_19__1_ <= n149;
    MEMORY_REG_19__0_ <= n154;
    MEMORY_REG_18__1_ <= n159;
    MEMORY_REG_18__0_ <= n164;
    MEMORY_REG_17__1_ <= n169;
    MEMORY_REG_17__0_ <= n174;
    MEMORY_REG_16__1_ <= n179;
    MEMORY_REG_16__0_ <= n184;
    MEMORY_REG_15__1_ <= n189;
    MEMORY_REG_15__0_ <= n194;
    MEMORY_REG_14__1_ <= n199;
    MEMORY_REG_14__0_ <= n204;
    MEMORY_REG_13__1_ <= n209;
    MEMORY_REG_13__0_ <= n214;
    MEMORY_REG_12__1_ <= n219;
    MEMORY_REG_12__0_ <= n224;
    MEMORY_REG_11__1_ <= n229;
    MEMORY_REG_11__0_ <= n234;
    MEMORY_REG_10__1_ <= n239;
    MEMORY_REG_10__0_ <= n244;
    MEMORY_REG_9__1_ <= n249;
    MEMORY_REG_9__0_ <= n254;
    MEMORY_REG_8__1_ <= n259;
    MEMORY_REG_8__0_ <= n264;
    MEMORY_REG_7__1_ <= n269;
    MEMORY_REG_7__0_ <= n274;
    MEMORY_REG_6__1_ <= n279;
    MEMORY_REG_6__0_ <= n284;
    MEMORY_REG_5__1_ <= n289;
    MEMORY_REG_5__0_ <= n294;
    MEMORY_REG_4__1_ <= n299;
    MEMORY_REG_4__0_ <= n304;
    MEMORY_REG_3__1_ <= n309;
    MEMORY_REG_3__0_ <= n314;
    MEMORY_REG_2__1_ <= n319;
    MEMORY_REG_2__0_ <= n324;
    MEMORY_REG_1__1_ <= n329;
    MEMORY_REG_1__0_ <= n334;
    MEMORY_REG_0__1_ <= n339;
    MEMORY_REG_0__0_ <= n344;
    NL_REG_3_ <= n349;
    NL_REG_2_ <= n353;
    NL_REG_1_ <= n357;
    NL_REG_0_ <= n361;
    SCAN_REG_4_ <= n365;
    SCAN_REG_3_ <= n370;
    SCAN_REG_2_ <= n375;
    SCAN_REG_1_ <= n380;
    SCAN_REG_0_ <= n385;
    MAX_REG_4_ <= n390;
    MAX_REG_3_ <= n395;
    MAX_REG_2_ <= n400;
    MAX_REG_1_ <= n405;
    MAX_REG_0_ <= n410;
    IND_REG_1_ <= n415;
    IND_REG_0_ <= n420;
    TIMEBASE_REG_5_ <= n425;
    TIMEBASE_REG_4_ <= n430;
    TIMEBASE_REG_3_ <= n435;
    TIMEBASE_REG_2_ <= n440;
    TIMEBASE_REG_1_ <= n445;
    TIMEBASE_REG_0_ <= n450;
    COUNT_REG2_5_ <= n455;
    COUNT_REG2_4_ <= n460;
    COUNT_REG2_3_ <= n465;
    COUNT_REG2_2_ <= n470;
    COUNT_REG2_1_ <= n475;
    COUNT_REG2_0_ <= n480;
    SOUND_REG_2_ <= n485;
    SOUND_REG_1_ <= n490;
    SOUND_REG_0_ <= n495;
    ADDRESS_REG_4_ <= n500;
    ADDRESS_REG_3_ <= n505;
    ADDRESS_REG_2_ <= n510;
    ADDRESS_REG_1_ <= n515;
    ADDRESS_REG_0_ <= n520;
    DATA_IN_REG_1_ <= n525;
    DATA_IN_REG_0_ <= n530;
    S_REG <= n535;
    PLAY_REG <= n540;
    NLOSS_REG <= n545;
    SPEAKER_REG <= n549;
    WR_REG <= n553;
    COUNTER_REG_2_ <= n558;
    COUNTER_REG_1_ <= n563;
    COUNTER_REG_0_ <= n568;
    COUNT_REG_1_ <= n573;
    NUM_REG_1_ <= n577;
    NUM_REG_0_ <= n581;
    DATA_OUT_REG_1_ <= n586;
    DATA_OUT_REG_0_ <= n591;
    GAMMA_REG_4_ <= n596;
    GAMMA_REG_3_ <= n601;
    GAMMA_REG_2_ <= n606;
    GAMMA_REG_1_ <= n611;
    GAMMA_REG_0_ <= n616;
  end
endmodule



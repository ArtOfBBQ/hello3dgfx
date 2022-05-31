#include "tok_random.h"

static uint32_t current_random_i = RANDOM_SEQUENCE_SIZE + 1;
static uint32_t random_sequence[RANDOM_SEQUENCE_SIZE] = {
301440837, 121456972, 403544619, 407074413, 122099976, 30687732, 414957239, 207939406, 231638950, 199832139, 207985599, 287371578, 327198916, 377795261, 105372600, 121963634, 345796375, 208736390, 51114921, 15823900, 242261848, 2280663, 165754724, 3885254, 222665007, 253406731, 91096248, 186246187, 280280670, 91997725, 242562205, 83828161, 188764401, 174551144, 18426412, 180271943, 130031614, 333601667, 229376639, 304342847, 158850907, 106400507, 216545756, 208925509, 362121017, 272027956, 388787454, 289843933, 257625418, 213487724, 3491407, 82415310, 334889373, 64358346, 276402193, 95251732, 276026696, 248701814, 113220474, 34317332, 30542689, 366087553, 239461075, 262335924, 362463180, 225649360, 127706566, 344918008, 428678809, 257440037, 13938768, 21564906, 111769075, 90014638, 225099534, 252766011, 85950691, 346461921, 192421089, 282330321, 304669515, 309423093, 128370715, 140577830, 345961170, 149661804, 344330147, 284116704, 180237047, 393166872, 189761340, 228566109, 304328209, 146392742, 105784477, 272079776, 94885022, 209051954, 134100239, 55100134, 120200372, 216644554, 409879664, 288138081, 225467825, 232990100, 300180510, 157488274, 108925513, 387107596, 103357096, 17452754, 4400959, 139578745, 389575678, 370008440, 104867540, 283199637, 177101021, 225674495, 356698999, 149463645, 394183533, 119658258, 186985957, 13242694, 212066443, 263468599, 31701906, 93158771, 94359604, 192765113, 344176800, 421457391, 46077634, 79131356, 34868332, 196258918, 133887115, 319354135, 246935672, 39556799, 173023875, 48481188, 267238683, 73573983, 49407377, 27168354, 236321978, 145999754, 172330963, 335395365, 400447991, 246385751, 65764832, 210704459, 60239238, 215117323, 92957677, 110632916, 364307569, 150866458, 101801803, 267771135, 377414215, 245569002, 350054927, 381205494, 195813722, 383327803, 174151425, 142041040, 140498100, 283044382, 157216719, 176007772, 342516649, 201460970, 139243532, 239104753, 94656749, 214964494, 20973347, 16894405, 187221305, 399694060, 304165132, 256931445, 151682259, 371906488, 170485000, 329557873, 12602460, 425908098, 90748870, 72741824, 384337783, 363371048, 21148028, 21521685, 346524635, 103383546, 149990357, 271733831, 6282379, 282970840, 337357934, 176458802, 136277406, 371591384, 206826503, 87405176, 364551238, 335747077, 167742307, 9055529, 342928066, 52420566, 275280040, 368935781, 143621894, 351857286, 3707288, 84501562, 36004229, 145836055, 365893862, 38463172, 3709773, 337765276, 250215367, 207471775, 270426048, 294778011, 339161541, 18246923, 363335069, 350565967, 181712696, 300593168, 143884613, 370383437, 366637521, 346254275, 108161288, 52918182, 306183973, 186946903, 288248334, 59114854, 212531612, 160051387, 268708925, 242577321, 243964784, 33885426, 250397570, 271461908, 93613569, 352269036, 7028343, 205613905, 322000263, 223620693, 313851045, 48095336, 394254930, 301336700, 218504187, 156071838, 36279869, 303940187, 403496777, 358267323, 136036584, 177874430, 54502228, 269077218, 53711391, 135369337, 347224704, 322412690, 188572634, 381086035, 414571788, 363546664, 15832288, 240276588, 20894184, 412747114, 8775873, 67778911, 234379803, 282554982, 426728998, 198944654, 218122630, 344679516, 378052515, 73131830, 82760989, 148126559, 63528087, 222029236, 149747418, 21049502, 9014855, 33201250, 38802429, 319515991, 281930208, 172769057, 191021510, 362406516, 335038913, 224392522, 240299576, 342349066, 88974132, 266176279, 71061534, 321905971, 244156489, 114272149, 215872560, 322631845, 156610498, 135324460, 71286893, 77490975, 170117649, 332874347, 209896029, 270044699, 155165909, 300333635, 199352951, 286758618, 376225344, 115187215, 374109763, 186849724, 293404933, 214254655, 367770706, 20168028, 86212374, 315488065, 396240507, 85449187, 393919055, 309163721, 156399630, 253778700, 216579790, 9049437, 285970483, 62490504, 139852208, 15356056, 412263535, 34856497, 390288476, 57171348, 291523309, 204799444, 415711239, 122964500, 58225791, 398652486, 204023823, 149764293, 76540765, 66885478, 127115726, 248056417, 267055829, 171164918, 313500982, 76493711, 228488705, 198721714, 55584690, 214123084, 240969138, 346242323, 336736648, 429399306, 12531060, 50454909, 135579269, 401393392, 219678751, 280822114, 412454004, 390025861, 149889906, 317360384, 15501917, 423493646, 291555433, 338249352, 252860743, 283706652, 230506677, 269525825, 263244725, 357482659, 293237598, 398116361, 148866334, 11175786, 266391562, 412698854, 114358020, 390611465, 332206284, 408453164, 17099882, 84845729, 307108696, 64541175, 391738536, 232774372, 394324286, 365276006, 25166371, 166820618, 246579886, 266797994, 343615957, 338346245, 8248325, 243078375, 96514088, 209872021, 302544526, 59908023, 208537129, 140672385, 330086451, 108966246, 246622590, 171984154, 163377387, 247668071, 236740420, 422193454, 163095062, 288839517, 205344674, 367313940, 366494313, 265097834, 142509583, 124660628, 126561947, 89497314, 411729596, 23302344, 387152872, 168329206, 386975591, 217718551, 163926001, 371290458, 224594997, 344810643, 257286807, 276574340, 350782738, 422407487, 150619053, 167386124, 364526991, 318941332, 370976437, 32851882, 52599487, 274085545, 34233029, 198375744, 176757298, 272752790, 192029404, 172484580, 110530244, 306469928, 23781035, 167074303, 34534546, 259333906, 309612781, 383744524, 5182647, 369946565, 26177263, 46238688, 250304450, 96451750, 47766965, 14656027, 378863242, 258030827, 167205457, 123184096, 137088024, 285204584, 224856206, 328709208, 199442702, 398290191, 193728784, 229366812, 169758358, 383783038, 322566623, 331468624, 314257528, 176063605, 152239972, 331047802, 304415693, 307097612, 369209738, 400688921, 249718538, 337591301, 226836106, 410650265, 241875700, 384390051, 271999669, 299214630, 103801379, 50238931, 11266412, 242477122, 86687337, 337121028, 232673644, 180054408, 376700954, 279479435, 363536708, 35702708, 67798440, 100048313, 224384035, 330034372, 48460128, 214912301, 216889921, 58956132, 171567044, 303425393, 122452994, 373903462, 338904940, 262548764, 396392990, 395110999, 119981721, 136388163, 238424854, 315481817, 164644661, 266658663, 330057004, 359449250, 279561086, 378777224, 93048322, 171220654, 203780176, 81822162, 353964796, 90338232, 86942912, 155484933, 161785020, 116040590, 348744855, 141578657, 127164245, 69561692, 312674659, 1733562, 337774337, 117997033, 264239871, 114978431, 237260251, 102696535, 277193108, 37311650, 17307823, 94197646, 360058029, 201037626, 245011637, 84644586, 17764865, 285671265, 139133373, 210251824, 242533480, 402060856, 157814711, 99082766, 163740251, 314807445, 263202574, 43133467, 350978076, 278671897, 72221237, 358642965, 198469117, 207584853, 368850031, 214306569, 233343054, 397997978, 249790136, 95998721, 350258325, 41702934, 228670265, 208554390, 192670500, 204258446, 412904151, 389227491, 119320132, 394244885, 233003840, 55725557, 222870492, 147374725, 186202110, 187943011, 251482521, 351691858, 52182450, 26476455, 321719010, 159200077, 286230712, 46942529, 51676146, 172286353, 144626782, 305551186, 260384298, 261293399, 386901468, 31768913, 345121925, 358383676, 372854875, 138839360, 312173169, 274070475, 427314853, 420859208, 274450904, 190807716, 197491965, 339314006, 137711152, 113642769, 290975587, 421438136, 82489465, 392399794, 8784010, 54361800, 116273856, 268803740, 263113407, 111715701, 48698048, 279372632, 72530537, 368731644, 361738452, 181085489, 379301840, 237853663, 273926786, 334967205, 139008623, 126704911, 273221381, 8314058, 338140267, 53922635, 169802791, 219547853, 412702082, 420997983, 104979622, 59193193, 24231041, 105642635, 301314033, 412131746, 40015110, 214915925, 13938493, 228698807, 348835322, 161531474, 206893454, 272277188, 400649754, 356593676, 250260833, 183799464, 187790190, 162316622, 236045784, 58298432, 338032640, 93366573, 60934689, 410565117, 411696037, 168387925, 189481705, 277708843, 226833124, 244970793, 298223925, 56610222, 255891877, 149905489, 129986633, 410264996, 4784086, 306192363, 12640738, 180354658, 168471337, 324541685, 261830997, 43223693, 387372055, 156850913, 197873961, 350716991, 300891866, 391644559, 95760568, 376228101, 345455605, 217092024, 149963491, 87508036, 310334337, 237783632, 319815200, 366010613, 278286283, 164965881, 167223346, 126322487, 76127633, 276495446, 327274792, 164106747, 17197612, 333134605, 335553456, 294742865, 221210765, 377060092, 178302913, 257411551, 95617548, 108179238, 245653310, 117354845, 60894857, 258029112, 323045665, 324562412, 342329253, 282794835, 377182078, 401856027, 270506831, 241592354, 49084897, 131329556, 196988155, 341314798, 410977930, 392297809, 403015586, 395771684, 416748555, 272263504, 173271792, 270610322, 212211364, 23788695, 392923371, 353249321, 37677075, 83284915, 59150622, 114653813, 291034140, 10062002, 216676873, 124967676, 239197614, 309196990, 141008031, 118844415, 228850305, 51624100, 427754723, 155722092, 215608379, 281765326, 420494065, 43717460, 316954458, 248293554, 22092581, 85337873, 21595028, 70078371, 63807669, 368997699, 282127242, 107556401, 177005134, 244946185, 120297973, 347906355, 290024253, 2063318, 176683910, 178289787, 342082652, 302067934, 284673509, 278050378, 57127161, 231042465, 129974393, 424903860, 83714824, 282863790, 187584674, 96135406, 136982295, 293364884, 213287452, 126895376, 353653437, 351649985, 128753649, 428962313, 47027097, 48939459, 279259735, 257086576, 24488255, 284494757, 234661315, 83614169, 295820365, 378183131, 104307398, 244511166, 39348153, 343854311, 283228995, 360389005, 243181298, 307609715, 181384435, 417102392, 422684736, 310331415, 213369256, 366516733, 340101498, 328603642, 128579599, 227749812, 128039903, 387733861, 112889101, 261363432, 65423803, 326256773, 395692070, 11306630, 408861571, 201029425, 64870737, 203637290, 354522741, 315205702, 224752009, 201962028, 123111519, 32671467, 380865035, 356030454, 144191012, 60443018, 285562105, 214633550, 162368175, 8143997, 257152092, 373410604, 216154783, 248910491, 404935163, 321435043, 119710918, 169399664, 391751144, 167691362, 302244129, 229286868, 93630096, 50204390, 91803365, 282170292, 157227969, 401433789, 392642455, 334592050, 234276364, 50612043, 305326887, 32560897, 149309505, 29228747, 397799126, 47841914, 320283600, 58513667, 232256948, 279628021, 339841485, 91668622, 95753952, 356236764, 88407986, 122491669, 141089947, 261812891, 91399030, 422802060, 419545840, 160459123, 54274249, 330818453, 424914681, 228370934, 317261940, 308907981, 166463729, 53460527, 235236304, 167901262, 412748736, 133496600, 205254149, 145308632, 46576637, 35344127, 255222026, 62103257, 119142396, 105339531, 2973624, 180191989, 117385364, 372113269, 291450953, 276155960, 254009154, 289233279, 222086330, 334609208, 426200986, 281209958, 222973360, 34486845, 332541303, 317634463, 41917416, 422016813, 256082142, 11570591, 100778500, 125975471, 262846143, 377438502, 312632311, 142495140, 307460429, 92917616, 281016281, 98315110, 34864835, 343164104, 95748644, 81297903, 428683229, 392922465, 154018013, 339999158, 239770157, 129587498, 198440915, 348257989, 151055908, 409008491, 200411190, 168252852, 270767679, 158581713, 259457914, 178656177, 11267776, 165157229, 175369187, 77249806, 166604107, 348367056, 36191445, 66780638, 72997409, 226833012, 327953198, 365664685, 62265549, 247669676, 294710151, 287018917, 36610755, 398609568, 35078458, 249948784, 396672223, 172308384, 32598857, 240251597, 377145340, 185557470, 160477486, 267225596, 161384517, 200951665, 359484051, 129082259, 168378758, 30733017, 347503160, 93765736, 353467951, 14554090, 273914624, 318595478, 162113508, 165899538, 142106619, 377423896, 107072213, 339893510, 78356000, 199741043, 264165363, 289562103, 347218723, 316540627, 398652425, 249428265, 251468519, 253646407, 151379631, 13925400, 23660955, 303832312, 19687188, 135692156, 114097249, 128643233, 174917035, 289834432, 67918373, 12323299, 170216575, 86726107, 130794726, 268474434, 25696168, 286027887, 248827716, 339941710, 427612045, 37774554, 13484333, 385719125, 418762690, 255366778, 134787932, 201873752, 106979219, 40497807, 395139890, 204954308, 407270431, 78489917, 4109105, 214390159, 296869443, 333512376, 282530262, 379446808, 358551493, 26953222, 101840853, 391473067, 374832060, 284990332, 354339149, 349195091, 190753089, 154814676, 422249363, 20802300, 32378736, 125063488, 230484823, 426883250, 261222469, 346856627, 246671965, 389757683, 288319750, 153401657, 138449281, 400163536, 337385915, 66640853, 68966117, 7256642, 202827307, 36150512, 81799749, 115014082, 424996353, 348019526, 149523752, 297976710, 348827326, 45588229, 65702108, 378895353, 169101822, 326125451, 328697681, 255797556, 307958158, 320991011, 12202345, 214978771, 273680254, 388473545, 55914642, 193598750, 379430184, 30875874, 141095866, 294479837, 269826309, 412576994, 190168723, 402505196, 26641546, 342283052, 58561707, 312620823, 180034227, 367201913, 152328816, 317295045, 224622287, 301335675, 254915735, 374537002, 162982141, 409778875, 317676279, 76653629, 326251489, 32854468, 86990757, 138071894, 174744304, 186209937, 186726582, 216169948, 304759395, 390314502, 360689554, 275514777, 309432993, 39717713, 54596806, 103664922, 302027770, 261503483, 5069185, 82899804, 40427176, 269918089, 407795566, 256097210, 183855048, 174451295, 125794198, 189339795, 400448047, 308390720, 292908001, 352405485, 78848478, 122190557, 108896030, 302540568, 429097311, 152211679, 328991283, 20244617, 256496476, 37210159, 387853921, 420291691, 176602133, 335647907, 294315960, 260337686, 85901053, 172853911, 217110263, 258074302, 35806130, 200736056, 213800537, 170872554, 319645114, 336011060, 238480807, 29351773, 63906309, 264488302, 200163593, 24576241, 346991365, 199815829, 274021566, 141334966, 195047594, 334009241, 69270868, 83271382, 325656254, 129816215, 62867177, 173672999, 354072617, 361314152, 375850130, 375582676, 422378704, 182223578, 68638979, 422000862, 236137000, 198661923, 205295787, 23339579, 209718355, 298588706, 149175007, 259767828, 133170211, 121316986, 428532023, 286501393, 172568782, 338222498, 387031981, 95214053, 161051652, 303753175, 17591611, 165176767, 281321029, 219057, 22565383, 290895811, 28482638, 59126318, 246437780, 320387163, 85062610, 225936238, 408755699, 154203299, 143112707, 113216935, 264012846, 308912130, 54667912, 181051687, 400751260, 297542510, 261777864, 123145050, 98456440, 30351122, 323704603, 397386168, 78835261, 188731535, 12359185, 274776103, 358639591, 229405629, 180754964, 305868169, 102000177, 259544404, 199494626, 20867841, 301725515, 410500077, 116516193, 315922657, 250805484, 196272733, 49203443, 380654735, 193503359, 224435699, 216677046, 209997974, 390565404, 176109610, 370999443, 29076752, 206699225, 274684160, 280179961, 384236376, 273588992, 43452332, 27622468, 416401605, 285842974, 49339948, 259909699, 120886903, 267452752, 218328912, 415340210, 337663637, 211351320, 400666321, 86495065, 199341473, 317444868, 128130673, 163812166, 290718251, 376725656, 188102262, 216321898, 316857120, 136531736, 117496287, 400830297, 38311401, 377216551, 183074276, 334695602, 173752013, 393447446, 172717870, 330467881, 422421159, 301865544, 340266034, 189910852, 205740715, 334517017, 395662469, 177598802, 97598393, 141473549, 32640511, 317693815, 249866868, 214385803, 377189012, 383871558, 46962831, 83831327, 274657932, 331240456, 214840872, 301677668, 238679116, 179340575, 172545343, 262382488, 118778156, 301090680, 263068823, 168466827, 39192562, 42734320, 283699561, 79180060, 400436478, 78235332, 66121904, 59040058, 228583386, 135812903, 67014714, 394081254, 126438210, 17684379, 257136704, 127246872, 70746701, 255935018, 48664087, 25503953, 249677202, 52427553, 118423880, 180643397, 295012602, 345604372, 237445952, 398971845, 386035072, 304699453, 351004129, 188719610, 36507992, 423481578, 366243554, 82319488, 202096461, 240156911, 176762199, 110789311, 17736947, 306893345, 23771920, 207962557, 94425622, 338541063, 238863163, 185204163, 267107155, 264647916, 174149956, 216798573, 308014838, 238210448, 268258907, 186670051, 55975225, 179072921, 121525124, 224510039, 86924410, 72602213, 10580861, 282761222, 313706535, 24383858, 307812183, 109148634, 80993372, 302649807, 232475321, 162576482, 175294390, 261738041, 205816029, 58858844, 145455182, 74884041, 315480941, 124423812, 221455324, 22306378, 170445886, 367028964, 319780348, 232781958, 322880275, 83084935, 428085701, 41917, 191572559, 175955097, 40337915, 58182135, 375977948, 205159204, 172605610, 63640000, 378504440, 180193074, 270044929, 237961163, 74637736, 421456084, 375772087, 308576754, 339858943, 235120115, 42619354, 356228630, 309744551, 375653195, 332709713, 111463402, 411026325, 245028550, 324894434, 46722500, 85934986, 121848429, 378656465, 422197429, 360938570, 252394394, 420934222, 49800704, 387402432, 52587560, 118702262, 365012990, 103518568, 224363097, 94116437, 291008128, 318252871, 183854780, 321429944, 151029137, 325232360, 274765669, 204439184, 338938720, 239962670, 308858283, 417282707, 222100051, 215560649, 404179385, 368478270, 333063375, 75367930, 6866275, 155780783, 144776753, 29721443, 97475751, 113830882, 257396311, 236695097, 177738266, 58316101, 403699073, 264771558, 114549423, 325994089, 354264580, 314372906, 259190051, 425912683, 175340123, 131193327, 417539513, 268688848, 405761478, 9522886, 340997876, 87636191, 217414466, 414143821, 105762837, 1939361, 324439785, 30174971, 292982380, 122420246, 266219177, 178309778, 83659791, 230328348, 341788912, 210070581, 172871929, 426059967, 235242323, 275950184, 18626713, 323770077, 83997823, 49947451, 371501280, 419410990, 165405582, 92193740, 142057017, 19012261, 361663673, 417839022, 47276015, 88192175, 183019975, 373750252, 148134227, 84966084, 65029763, 211419399, 361317392, 331614872, 336511196, 298532344, 408335843, 73001052, 80369591, 268809811, 94357879, 197425349, 10446596, 422458173, 390292665, 228406309, 385287974, 143641227, 309384350, 4454939, 335908920, 389924311, 25221055, 63449350, 224023606, 142699926, 179555495, 243102832, 400612534, 155949751, 237145256, 306494937, 168086405, 89668840, 75044822, 100428247, 62355095, 321365840, 306580340, 89382074, 339653834, 300178424, 158390635, 154274009, 279575228, 264699023, 71797243, 284522340, 389758822, 89281359, 53710436, 51537064, 330115111, 295991372, 336591240, 52169539, 289744015, 160913426, 108060044, 410743210, 204531902, 70315445, 309224737, 332079234, 710791, 401748560, 277308761, 409066289, 59894139, 271325109, 56507965, 70552140, 341336410, 224902374, 326138466, 92244425, 239423569, 219408304, 250806771, 315720389, 67103227, 136376105, 250177882, 391620561, 4817672, 3903908, 85471023, 413463148, 197255749, 213732874, 56332754, 138780445, 67172304, 217364311, 396227219, 238391121, 156488248, 337619546, 193991852, 195127636, 115793131, 7677269, 169649051, 19733007, 287814024, 173809398, 367479820, 364817854, 269494714, 132948453, 156760272, 2880806, 78301352, 427438630, 353853068, 340103754, 136092665, 202500957, 333344197, 283161477, 13740663, 345180813, 417915284, 120503331, 310170792, 279147175, 27156048, 236520497, 202876019, 426607717, 403283892, 191942112, 28435673, 428683194, 135352367, 266009707, 100943599, 89969642, 127667194, 380329042, 308941180, 227609039, 399347018, 302844071, 21624634, 346095378, 162276044, 9579528, 190459707, 243629283, 60248783, 256862553, 119695536, 284968036, 52481142, 295418830, 308074833, 88614391, 159065655, 75236186, 390778671, 35935505, 83232432, 131934155, 2467358, 266365602, 10366663, 169266513, 69300558, 335284685, 403596999, 355864063, 215209964, 77719388, 186711715, 391490568, 117542708, 200467130, 419123735, 313551469, 314127899, 34330201, 44922448, 228773818, 337178104, 222848314, 398594745, 319816333, 113651740, 107887117, 118726945, 296566046, 123448218, 79243430, 324242660, 321446215, 28286101, 301556887, 392656274, 30652340, 361544581, 189224181, 212698957, 110912287, 16318030, 137932156, 119365422, 244425630, 36639770, 361205151, 346308745, 25142503, 20256480, 153887969, 155275848, 140166891, 192397439, 286490271, 121159031, 205246981, 47678796, 94919675, 232033526, 385675353, 355849423, 247827851, 93024386, 327253676, 104812351, 203234806, 136712840, 76724644, 268957239, 68125679, 140979287, 281131150, 54190545, 361634083, 178249071, 51950776, 246020727, 427325728, 407514188, 60487487, 408954426, 180469591, 198494993, 359058510, 351731829, 14609644, 163986642, 325576087, 322032822, 400376723, 133376625, 266844586, 426963622, 267647093, 120018659, 215610753, 398308062, 202738378, 410583876, 227632444, 345712120, 23353366, 102990474, 51818706, 385763341, 416750570, 65836904, 220587862, 260020142, 108567990, 340524783, 51594175, 161820717, 176926393, 143933693, 70030424, 122441246, 309722650, 380357466, 92546749, 390622715, 238640263, 221777952, 261291775, 148553660, 254413025, 25507061, 192305400, 32709543, 348792324, 388575097, 301993450, 22543129, 101271624, 200891015, 277518672, 396980626, 414430096, 25983428, 427072853, 289975756, 358028756, 242284382, 128587738, 374300939, 272582089, 218806427, 404874850, 71064997, 396785731, 261490146, 216211310, 311173744, 373416669, 143769715, 108171251, 346933013, 38848436, 172066965, 76648222, 247377280, 247375648, 384767388, 307784300, 266242399, 340011801, 25962252, 258107457, 275660566, 257879763, 230831741, 274907117, 66527086, 289872214, 55122338, 183774648, 10778149, 270742454, 377438489, 188323314, 226987633, 81792759, 233686206, 9064065, 269516074, 206514131, 265351531, 9477919
};

uint32_t tok_rand() {
    
    if (current_random_i == RANDOM_SEQUENCE_SIZE + 1) {
        // rng wasn't initialized yet
        current_random_i =
            (uint32_t)(platform_get_current_time_microsecs() %
                RANDOM_SEQUENCE_SIZE);
    }
    
    current_random_i++;
    if (current_random_i >= RANDOM_SEQUENCE_SIZE) {
        current_random_i = 0;
    }
    
    return random_sequence[current_random_i];
}

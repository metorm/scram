variable
   bool  g1, g2, g3, g4, g5, e4, e5, e2, e3, g38, e1, g6, g7, g8, g9, e6, g10, g11, g12, g13, e7, g14, g15, g16, g17, e9, e8, g18, g19, g20, g21, e10, g22, g23, g24, g25, e11, g26, g27, g28, g29, e13, e12, g30, g31, g32, g33, e14, g34, g35, g36, g37, e15, g39, g40, e16, e17, g41, g42, e18, e19, g43, e20, e21, g44, g45, g46, g47, g48, e22, g49, g50, g51, e23, g52, e24, e25, g53, e26, g54, e27, g55, g56, g57, g58, g59, g60, g61, g62, g63, g64, g65, g66, g67, g68, g69, g70, e28, g71, e29, g133, g134, g135, g72, g73, g74, g75, e30, g76, e31, g127, g128, g129, g77, g78, g79, g80, e32, g81, e33, g139, g140, g141, g82, g83, g84, g85, e34, g86, e35, g87, g88, g89, g90, e36, g91, e37, g92, g93, g94, g95, e38, g96, e39, g97, g98, g99, g100, e40, g101, e41, g102, g103, g104, g105, e42, g106, e43, g107, g108, g109, g110, e44, g111, e45, g112, g113, g114, e46, g115, g116, e47, g117, g118, g119, e48, g120, g121, e49, g122, g123, g124, e50, g125, g126, e51, e52, g130, e53, g131, e54, g132, e67, g168, e64, g165, e61, g162, e55, g136, e56, g137, e57, g138, e68, g169, e65, g166, e62, g163, e58, g142, e59, g143, e60, g144, e69, g170, e66, g167, e63, g164, g145, g146, g147, g148, g149, g150, g151, g152, g267, g153, g266, g265, g264, g154, g155, g156, g157, g158, g159, g160, g271, g270, g269, g268, g161, g189, g198, g199, g190, g205, g206, g191, g212, g213, g192, g219, g220, g193, g223, g224, g194, g227, g228, g195, g231, g232, g196, g235, g236, g197, g239, g240, g171, e70, e71, g172, e72, g173, e73, g174, g175, g176, g177, g178, g179, g180, e74, g181, g182, g183, g184, g185, g186, g187, g188, g200, g201, g202, g203, g204, g244, e117, g245, e120, g243, e114, e75, e76, e93, e94, g207, g208, g209, g210, g211, e118, e121, e115, e77, e78, e95, e96, g214, g215, g216, g217, g218, e119, e122, e116, e79, e80, e97, e98, g221, g222, e81, e82, e99, e100, g225, g226, e83, e84, e101, e102, g229, g230, e85, e86, e103, e104, g233, g234, e87, e88, e105, e106, g237, g238, e89, e90, e107, e108, g241, g242, e91, e92, e109, e110, e111, e112, e113, g246, g247, g248, g249, g250, g251, g252, g253, g254, g255, g256, g257, g258, g259, g260, g261, g262, g263, g275, g274, g272, g273, g279, g278, g276, g277, g283, g282, g280, g281, g287, g286, g284, g285, r1;

rule
   (r1==(g161 && g145));
   (g1==(!g2));
   (g2==(g3 || g4 || g5));
   (g3==(e4 || e5));
   (g4==(e2 || e3));
   (g5==(g38 || e1));
   (g6==(!g7));
   (g7==(g3 || g8 || g9));
   (g8==(e2 || e3));
   (g9==(g38 || e6));
   (g10==(!g11));
   (g11==(g3 || g12 || g13));
   (g12==(g38 || e7));
   (g13==(e2 || e3));
   (g14==(!g15));
   (g15==(g3 || g16 || g17));
   (g16==(e2 || e9));
   (g17==(g38 || e8));
   (g18==(!g19));
   (g19==(g3 || g20 || g21));
   (g20==(e2 || e9));
   (g21==(g38 || e10));
   (g22==(!g23));
   (g23==(g3 || g24 || g25));
   (g24==(g38 || e11));
   (g25==(e2 || e9));
   (g26==(!g27));
   (g27==(g3 || g28 || g29));
   (g28==(e13 || e2));
   (g29==(e12 || g38));
   (g30==(!g31));
   (g31==(g3 || g32 || g33));
   (g32==(e13 || e2));
   (g33==(e14 || g38));
   (g34==(!g35));
   (g35==(g3 || g36 || g37));
   (g36==(e15 || g38));
   (g37==(e13 || e2));
   (g38==(e4 || e5));
   (g39==(!g40));
   (g40==(e16 || e17));
   (g41==(!g42));
   (g42==(e18 || e19));
   (g43==(e20 || e21 || g44 || e1));
   (g44==(e16 || e17));
   (g45==(e20 || e21 || g44 || e8));
   (g46==(e20 || g44 || e21 || e12));
   (g47==(g48 || e22 || e20 || e6));
   (g48==(e18 || e19));
   (g49==(g48 || e22 || e20 || e10));
   (g50==(g48 || e22 || e20 || e14));
   (g51==(e20 || e23 || g52 || e24));
   (g52==(e4 || e25));
   (g53==(e20 || e23 || g52 || e26));
   (g54==(e20 || g52 || e23 || e27));
   (g55==( 2 == ( g47 + g51 + g43 )) );
   (g56==( 2 == ( g49 + g53 + g45 )) );
   (g57==( 2 == ( g50 + g54 + g46 )) );
   (g58==(g1 && g55));
   (g59==(g14 && g56));
   (g60==(g26 && g57));
   (g61==(g6 && g55));
   (g62==(g18 && g56));
   (g63==(g30 && g57));
   (g64==(g10 && g55));
   (g65==(g22 && g56));
   (g66==(g34 && g57));
   (g67==((g68 || g69) && (!(g68 && g69))));
   (g68==(g70 || e28));
   (g69==(g71 || e29));
   (g70==( 2 == ( g133 + g134 + g135 )) );
   (g71==( 2 == ( g133 + g134 + g135 )) );
   (g72==((g73 || g74) && (!(g73 && g74))));
   (g73==(g75 || e30));
   (g74==(g76 || e31));
   (g75==( 2 == ( g127 + g128 + g129 )) );
   (g76==( 2 == ( g127 + g128 + g129 )) );
   (g77==((g78 || g79) && (!(g78 && g79))));
   (g78==(g80 || e32));
   (g79==(g81 || e33));
   (g80==( 2 == ( g139 + g140 + g141 )) );
   (g81==( 2 == ( g139 + g140 + g141 )) );
   (g82==((g83 || g84) && (!(g83 && g84))));
   (g83==(g85 || e34));
   (g84==(g86 || e35));
   (g85==( 2 == ( g133 + g134 + g135 )) );
   (g86==( 2 == ( g133 + g134 + g135 )) );
   (g87==((g88 || g89) && (!(g88 && g89))));
   (g88==(g90 || e36));
   (g89==(g91 || e37));
   (g90==( 2 == ( g127 + g128 + g129 )) );
   (g91==( 2 == ( g127 + g128 + g129 )) );
   (g92==((g93 || g94) && (!(g93 && g94))));
   (g93==(g95 || e38));
   (g94==(g96 || e39));
   (g95==( 2 == ( g139 + g140 + g141 )) );
   (g96==( 2 == ( g139 + g140 + g141 )) );
   (g97==((g98 || g99) && (!(g98 && g99))));
   (g98==(g100 || e40));
   (g99==(g101 || e41));
   (g100==( 2 == ( g139 + g140 + g141 )) );
   (g101==( 2 == ( g139 + g140 + g141 )) );
   (g102==((g103 || g104) && (!(g103 && g104))));
   (g103==(g105 || e42));
   (g104==(g106 || e43));
   (g105==( 2 == ( g127 + g128 + g129 )) );
   (g106==( 2 == ( g127 + g128 + g129 )) );
   (g107==((g108 || g109) && (!(g108 && g109))));
   (g108==(g110 || e44));
   (g109==(g111 || e45));
   (g110==( 2 == ( g133 + g134 + g135 )) );
   (g111==( 2 == ( g133 + g134 + g135 )) );
   (g112==((g113 || g114) && (!(g113 && g114))));
   (g113==(e46 || g115));
   (g114==(g116 || e47));
   (g115==( 2 == ( g139 + g140 + g141 )) );
   (g116==( 2 == ( g139 + g140 + g141 )) );
   (g117==((g118 || g119) && (!(g118 && g119))));
   (g118==(e48 || g120));
   (g119==(g121 || e49));
   (g120==( 2 == ( g127 + g128 + g129 )) );
   (g121==( 2 == ( g127 + g128 + g129 )) );
   (g122==((g123 || g124) && (!(g123 && g124))));
   (g123==(e50 || g125));
   (g124==(g126 || e51));
   (g125==( 2 == ( g133 + g134 + g135 )) );
   (g126==( 2 == ( g133 + g134 + g135 )) );
   (g127==(e52 || g130));
   (g128==(e53 || g131));
   (g129==(e54 || g132));
   (g130==(e67 || g168));
   (g131==(e64 || g165));
   (g132==(e61 || g162));
   (g133==(e55 || g136));
   (g134==(e56 || g137));
   (g135==(e57 || g138));
   (g136==(e68 || g169));
   (g137==(e65 || g166));
   (g138==(e62 || g163));
   (g139==(e58 || g142));
   (g140==(e59 || g143));
   (g141==(e60 || g144));
   (g142==(e69 || g170));
   (g143==(e66 || g167));
   (g144==(e63 || g164));
   (g145==(g146 && g147));
   (g146==(g148 || g149));
   (g147==(g150 || g151));
   (g148==(g152 && g267));
   (g149==(g153 && g266));
   (g150==(g152 && g265));
   (g151==(g153 && g264));
   (g152==(!e16));
   (g153==(!e18));
   (g154==(g155 && g156));
   (g155==(g157 || g158));
   (g156==(g159 || g160));
   (g157==(g153 && g271));
   (g158==(g152 && g270));
   (g159==(g153 && g269));
   (g160==(g152 && g268));
   (g161==(!g154));
   (g162==(g189 || g198 || g58 || g199));
   (g163==(g190 || g205 || g59 || g206));
   (g164==(g191 || g212 || g60 || g213));
   (g165==(g192 || g219 || g61 || g220));
   (g166==(g193 || g223 || g62 || g224));
   (g167==(g194 || g227 || g63 || g228));
   (g168==(g195 || g231 || g232 || g64));
   (g169==(g196 || g235 || g236 || g65));
   (g170==(g197 || g239 || g240 || g66));
   (g171==(e70 || e71));
   (g172==(e70 || e72));
   (g173==(e73 || e70));
   (g174==(e70 || e71));
   (g175==(e70 || e72));
   (g176==(e73 || e70));
   (g177==(e70 || e71));
   (g178==(e70 || e72));
   (g179==(e73 || e70));
   (g180==(e74 || e7));
   (g181==(e74 || e11));
   (g182==(e15 || e74));
   (g183==(e74 || e7));
   (g184==(e74 || e11));
   (g185==(e15 || e74));
   (g186==(e74 || e7));
   (g187==(e74 || e11));
   (g188==(e15 || e74));
   (g189==(g180 && g171));
   (g190==(g181 && g172));
   (g191==(g182 && g173));
   (g192==(g183 && g174));
   (g193==(g184 && g175));
   (g194==(g185 && g176));
   (g195==(g186 && g177));
   (g196==(g187 && g178));
   (g197==(g188 && g179));
   (g198==( 2 == ( g200 + g201 + g202 )) );
   (g199==(g203 && g204));
   (g200==(g244 || e117));
   (g201==(g245 || e120));
   (g202==(g243 || e114));
   (g203==(e75 || e76));
   (g204==(e93 || e94));
   (g205==( 2 == ( g207 + g208 + g209 )) );
   (g206==(g210 && g211));
   (g207==(g244 || e118));
   (g208==(g245 || e121));
   (g209==(g243 || e115));
   (g210==(e77 || e78));
   (g211==(e95 || e96));
   (g212==( 2 == ( g214 + g215 + g216 )) );
   (g213==(g217 && g218));
   (g214==(e119 || g244));
   (g215==(e122 || g245));
   (g216==(e116 || g243));
   (g217==(e79 || e80));
   (g218==(e97 || e98));
   (g219==( 2 == ( g200 + g201 + g202 )) );
   (g220==(g221 && g222));
   (g221==(e81 || e82));
   (g222==(e99 || e100));
   (g223==( 2 == ( g207 + g208 + g209 )) );
   (g224==(g225 && g226));
   (g225==(e83 || e84));
   (g226==(e101 || e102));
   (g227==( 2 == ( g214 + g215 + g216 )) );
   (g228==(g229 && g230));
   (g229==(e85 || e86));
   (g230==(e103 || e104));
   (g231==( 2 == ( g200 + g201 + g202 )) );
   (g232==(g233 && g234));
   (g233==(e87 || e88));
   (g234==(e105 || e106));
   (g235==( 2 == ( g207 + g208 + g209 )) );
   (g236==(g237 && g238));
   (g237==(e89 || e90));
   (g238==(e107 || e108));
   (g239==(g241 && g242));
   (g240==( 2 == ( g214 + g215 + g216 )) );
   (g241==(e91 || e92));
   (g242==(e109 || e110));
   (g243==(e16 || e17 || e111));
   (g244==(e18 || e19 || e112));
   (g245==(e4 || e25 || e113));
   (g246==(g40 || g67));
   (g247==(g42 || g77));
   (g248==(g40 || g82));
   (g249==(g42 || g92));
   (g250==(g42 || g97));
   (g251==(g40 || g107));
   (g252==(g42 || g112));
   (g253==(g40 || g122));
   (g254==(g72 && g67));
   (g255==(g87 && g82));
   (g256==(g97 && g102));
   (g257==(g112 && g117));
   (g258==(g3 && g40));
   (g259==(g42 && g3));
   (g260==(e16 || g258 || g254));
   (g261==(e16 || g258 || g255));
   (g262==(e18 || g259 || g256));
   (g263==(e18 || g259 || g257));
   (g264==(g275 || g274));
   (g265==(g272 || g273));
   (g266==(g279 || g278));
   (g267==(g276 || g277));
   (g268==(g283 || g282));
   (g269==(g280 || g281));
   (g270==(g287 || g286));
   (g271==(g284 || g285));
   (g272==(g39 && g68 && g69));
   (g273==(g246 && g73 && g74));
   (g274==(g78 && g79 && g260));
   (g275==(g73 && g74 && g260 && g247));
   (g276==(g39 && g83 && g84));
   (g277==(g248 && g88 && g89));
   (g278==(g93 && g94 && g261));
   (g279==(g88 && g89 && g261 && g249));
   (g280==(g41 && g98 && g99));
   (g281==(g250 && g103 && g104));
   (g282==(g108 && g109 && g262));
   (g283==(g103 && g104 && g262 && g251));
   (g284==(g41 && g113 && g114));
   (g285==(g252 && g118 && g119));
   (g286==(g123 && g263 && g124));
   (g287==(g118 && g119 && g263 && g253));
variable
   bool  r1, g1, g2, g3, g10, g11, g22, g23, g4, g5, g6, g7, g8, g9, g47, g48, g60, g61, g45, g46, g62, g63, g12, g13, e1, g14, g15, e2, g16, e3, g66, e37, e38, e4, g67, e39, e40, g17, g18, g19, e5, g20, g21, e6, g68, g78, e49, g33, g36, g87, g88, g24, g25, g26, g27, g28, g29, g37, g30, g31, g38, g32, e7, e8, g34, g35, e9, e10, g92, g93, e62, g94, e63, g79, e50, g98, g99, g100, g101, e69, g102, g103, g104, g39, g40, g41, g42, g43, e11, e13, e15, g44, e12, e16, e18, e14, e17, g49, g52, g53, g54, g55, g50, g51, e19, e20, g56, e21, g59, e25, e24, g57, e22, e28, e27, g58, e23, e26, e65, e66, e67, e68, g64, e29, e33, e32, g65, e30, e36, e35, e31, e34, g69, e41, g70, e42, g71, g72, g73, g74, e43, g76, e45, g75, e44, g77, e46, e47, e48, g80, g81, g82, g83, e51, g85, e53, g84, e52, g86, e54, e55, e56, g89, e57, g90, e58, g91, e59, e60, e61, g95, g96, e64, g97, e75, e76, e74, e80, e72, e73, g105, e70, e77, e78, e71, e79, g106;

rule
   (r1==(g1 || g2 || g3));
   (g1==(g10 && g11));
   (g2==(g22 && g23));
   (g3==(g4 && g5));
   (g4==(g6 || g7));
   (g5==(g8 || g9));
   (g6==(g47 && g48));
   (g7==(g60 && g61));
   (g8==(g45 && g46));
   (g9==(g62 && g63));
   (g10==(g12 || g13 || e1));
   (g11==(g14 || g15 || e2));
   (g12==(g16 && e3));
   (g13==(g66 || e37 || e38));
   (g14==(g16 && e4));
   (g15==(g67 || e39 || e40));
   (g16==(g17 && g18));
   (g17==(g19 || e5));
   (g18==(g20 || g21 || e6));
   (g19==(g68 || g78 || e49));
   (g20==(g33 && g36));
   (g21==(g87 && g88));
   (g22==(g24 || g25));
   (g23==(g26 || g27));
   (g24==(g28 && g29));
   (g25==(g37 || g6));
   (g26==(g30 && g31));
   (g27==(g38 || g8));
   (g28==(g19 || g32 || e7));
   (g29==(g20 || g21 || g33 || e8));
   (g30==(g34 || g35 || e9));
   (g31==(g20 || g21 || g36 || e10));
   (g32==(g92 || g93 || e62));
   (g33==(g94 || g93 || e63));
   (g34==(g68 || g79 || e50));
   (g35==(g98 || g99 || g100 || g101 || e69));
   (g36==(g102 || g103 || g104 || g101 || e69));
   (g37==(g39 && g40));
   (g38==(g41 && g42));
   (g39==(g43 && e11));
   (g40==(g20 || g21 || e13 || e15));
   (g41==(g44 && e12));
   (g42==(g20 || g21 || e16 || e18));
   (g43==(g19 || e13 || e14));
   (g44==(g34 || e16 || e17));
   (g45==(g49 || g48));
   (g46==(g52 && g53));
   (g47==(g49 || g46));
   (g48==(g54 && g55));
   (g49==(g50 && g51));
   (g50==(g20 || g21 || e19));
   (g51==(g34 || e20));
   (g52==(g56 && e21));
   (g53==(g59 || e25 || e24));
   (g54==(g57 && e22));
   (g55==(g33 || e28 || e27));
   (g56==(g58 || e23 || e24));
   (g57==(g32 || e26 || e27));
   (g58==(g35 || e65 || e66));
   (g59==(g36 || e67 || e68));
   (g60==(g64 && e29));
   (g61==(g20 || g21 || e33 || e32));
   (g62==(g65 && e30));
   (g63==(g20 || g21 || e36 || e35));
   (g64==(g19 || e31 || e32));
   (g65==(g19 || e34 || e35));
   (g66==(g68 || g69 || e41));
   (g67==(g68 || g70 || e42));
   (g68==(g32 && g35));
   (g69==(g71 && g72));
   (g70==(g73 && g74));
   (g71==(g74 || e43));
   (g72==(g76 || g32 || e45));
   (g73==(g72 || e43));
   (g74==(g75 || g35 || e44));
   (g75==(g77 && e46));
   (g76==(g77 && e47));
   (g77==(g32 || g35 || e38 || e48 || e40));
   (g78==(g80 && g81));
   (g79==(g82 && g83));
   (g80==(g83 || e51));
   (g81==(g85 || g32 || e53));
   (g82==(g81 || e51));
   (g83==(g84 || g35 || e52));
   (g84==(g86 && e54));
   (g85==(g86 && e55));
   (g86==(g32 || g35 || e56));
   (g87==(g89 || g36 || e57));
   (g88==(g90 || g33 || e58));
   (g89==(g91 && e59));
   (g90==(g91 && e60));
   (g91==(g33 || g36 || e61));
   (g92==(g95 && g96));
   (g93==(g96 && e64));
   (g94==(g97 && e64));
   (g95==(e75 || e76 || e74));
   (g96==(e2 || e80 || e1));
   (g97==(e72 || e73 || e74));
   (g98==(g105 && e70));
   (g99==(e77 && e65));
   (g100==(e78 && e65));
   (g101==(e70 && e71));
   (g102==(e78 && e67));
   (g103==(e79 && e67));
   (g104==(g106 && e71));
   (g105==(e79 || e67 || e78));
   (g106==(e77 || e65 || e78));
C MSIS86.FOR	D. BILITZA	10/88
C
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
C SUBROUTINES AND FUNCTIONS:
C	GTS5, DENSS, GLOBE5, TSELEC, GLOB5L, DNET, CCOR, PRMSG5,GGM
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
C
C
C***********************************************************************
      SUBROUTINE GTS5(IYD,SEC,ALT,GLAT,GLONG,STL,F107A,F107,AP,MASS,D,T)
C        MSIS-86/CIRA 1986 Neutral Thermosphere Model
C         A.E.Hedin 3/15/85;2/26/87 (Variable Names Shortened)
C         10/14/87 increase altitude limit of O mixing calculation
C             ALTL(2) from 300.0 to 400.0 km .
C     INPUT:
C        IYD - YEAR AND DAY AS YYYYDDD
C        SEC - UT(SEC)
C        ALT - ALTITUDE(KM) (GREATER THAN 85 KM)
C        GLAT - GEODETIC LATITUDE(DEG)
C        GLONG - GEODETIC LONGITUDE(DEG)
C        STL - LOCAL APPARENT SOLAR TIME(HRS)
C        F107A - 3 MONTH AVERAGE OF F10.7 FLUX
C        F107 - DAILY F10.7 FLUX FOR PREVIOUS DAY
C        AP - MAGNETIC INDEX(DAILY) OR WHEN SW(9)=-1. :
C           - ARRAY CONTAINING:
C             (1) DAILY AP
C             (2) 3 HR AP INDEX FOR CURRENT TIME
C             (3) 3 HR AP INDEX FOR 3 HRS BEFORE CURRENT TIME
C             (4) 3 HR AP INDEX FOR 6 HRS BEFORE CURRENT TIME
C             (5) 3 HR AP INDEX FOR 9 HRS BEFORE CURRENT TIME
C             (6) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 12 TO 33 HRS PRIOR
C                    TO CURRENT TIME
C             (7) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 36 TO 59 HRS PRIOR
C                    TO CURRENT TIME
C        MASS - MASS NUMBER (ONLY DENSITY FOR SELECTED GAS IS
C                 CALCULATED.  MASS 0 IS TEMPERATURE.  MASS 48 FOR ALL.
C     OUTPUT: 
C        D(1) - HE NUMBER DENSITY(CM-3)
C        D(2) - O NUMBER DENSITY(CM-3)
C        D(3) - N2 NUMBER DENSITY(CM-3)
C        D(4) - O2 NUMBER DENSITY(CM-3)
C        D(5) - AR NUMBER DENSITY(CM-3)
C        D(6) - TOTAL MASS DENSITY(GM/CM3)
C        D(7) - H NUMBER DENSITY(CM-3)
C        D(8) - N NUMBER DENSITY(CM-3)
C        T(1) - EXOSPHERIC TEMPERATURE
C        T(2) - TEMPERATURE AT ALT
C
C      TO GET OUTPUT IN M-3 and KG/M3:   CALL METERS(.TRUE.) 
C
C          ADDITIONAL COMMENTS
C           (1) LOWER BOUND QUANTITIES IN COMMON/GTS3C/
C           (2) TO TURN ON AND OFF PARTICULAR VARIATIONS CALL TSELEC(SW)
C               WHERE SW IS A 25 ELEMENT ARRAY CONTAINING 0. FOR OFF, 1. 
C               FOR ON, OR 2. FOR MAIN EFFECTS OFF BUT CROSS TERMS ON
C               FOR THE FOLLOWING VARIATIONS
C               1 - F10.7 EFFECT ON MEAN  2 - TIME INDEPENDENT
C               3 - SYMMETRICAL ANNUAL    4 - SYMMETRICAL SEMIANNUAL
C               5 - ASYMMETRICAL ANNUAL   6 - ASYMMETRICAL SEMIANNUAL
C               7 - DIURNAL               8 - SEMIDIURNAL
C               9 - DAILY AP             10 - ALL UT/LONG EFFECTS
C              11 - LONGITUDINAL         12 - UT AND MIXED UT/LONG
C              13 - MIXED AP/UT/LONG     14 - TERDIURNAL
C              15 - DEPARTURES FROM DIFFUSIVE EQUILIBRIUM
C              16 - ALL TINF VAR         17 - ALL TLB VAR
C              18 - ALL T0 VAR           19 - ALL S VAR
C              20 - ALL Z0 VAR           21 - ALL NLB VAR
C              22 - ALL TR12 VAR         23 - TURBO SCALE HEIGHT VAR
C
C              To get current values of SW: CALL TRETRV(SW)
C
C !!!!!!!!!!!!!!!!!!!!!!!!!!! CHANGES !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
C 
C  - NAME,ISD,IST,ISDATE, and ISTIME were changed to character variables 
C    in GTS5 and PRMSG5
C		
C  - The variable dimension of P and AP in GLOBE5 and GLOBE5L was 
C    indicted by *, rather than 1; if this does not work on your system 
C    you may want to use P(150) and AP(7).
C		
C  - The large data statement in PRMSG5 is now read in from file 
C    MSIS86.DAT; some compilers do not allow named commons to be 
C    initialized in a data statement.
C
C  - The first call to GLOBE5 should come before the common array SW(25)
C    is used in GTS5.
C
C Dieter Bilitza !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! March 87
C **********************************************************************
      LOGICAL METER
      DIMENSION D(8),T(2),MT(10),AP(7),ALTL(8)
      CHARACTER NAME(2)*4
      COMMON/UINR/IUMSIS,MONITO,IIEE
      COMMON/GTS3C/TLB,S,DB04,DB16,DB28,DB32,DB40,DB48,DB01,ZA,T0,Z0
     $ ,G0,RL,DD,DB14
      COMMON/LOWER5/PTM(8),PDM(8,7)
      COMMON/PARM5/PT(150),PD(150,7),PS(150),PDL(25,2)
      COMMON/CSW/SW(25),ISW,SWC(25)
      COMMON/TTEST/TINFG,GB,ROUT,TT(15)
      COMMON/DATIME/ISDATE(3),ISTIME(2),NAME
      DATA MT/48,0,4,16,28,32,40,1,49,14/,IFL/0/
      DATA ALTL/200.,400.,150.,200.,240.,450.,320.,450./
      DATA IMR/0/
c
      IF(IFL.EQ.0) THEN
	CALL PRMSG5
	IF(IIEE.GT.0) GOTO 9999
      	IFL=1
      ENDIF
      YRD=IYD
C       Eq. A7
C!!OLD!! TINF=PTM(1)*(1.+SW(16)*GLOBE5(YRD,SEC,GLAT,GLONG,STL,F107A,F107,
C!!OLD!!$ AP,PT))*PT(1)
      GGGG=GLOBE5(YRD,SEC,GLAT,GLONG,STL,F107A,F107,AP,PT)
      TINF=PTM(1)*(1.+SW(16)*GGGG)*PT(1)
      ZA=PTM(5)*PDL(16,2)
C       Eq. A9
      T0=PTM(3)*PD(76,3)*(1.+SW(18)*GLOB5L(PD(76,3)))
C       Eq. A8
      TLB=PTM(2)*(1.+SW(17)*GLOB5L(PD(26,3)))*PD(26,3)
C       Eq. A10
      Z0=PTM(7)*(1.+SW(20)*GLOB5L(PD(51,3)))*PD(51,3)
C       Eq. A6
      G0=PTM(4)*PS(1)
     $ *(1.+SW(19)*GLOBE5(YRD,SEC,GLAT,GLONG,STL,F107A,F107, 
     $ AP,PS))
C       Eq. A5
      S=G0/(TINF-TLB)
C       Eq. A11
      TR12=PD(101,3)*(1.+SW(22)*GLOB5L(PD(101,3)))
      T(1)=TINF
      IF(MASS.EQ.0) GO TO 50
C       Eq. A18  N2
      G28=SW(21)*GLOB5L(PD(1,3))
      YRD=IYD
      T(1)=TINF
      XMM=PDM(5,3)
      DO 10 J = 1,10
      IF(MASS.EQ.MT(J))   GO TO 15
   10 CONTINUE
      WRITE(6,100) MASS
      GO TO 90
   15 IF(ALT.GT.ALTL(6).AND.MASS.NE.28.AND.MASS.NE.48) GO TO 17
C
C       **** N2 DENSITY ****
C
C       Eq. A18
      DB28 = PDM(1,3)*EXP(G28)*PD(1,3)
C       Eq. A13 - A17
      D(3)=DENSS(ALT,DB28,TINF,TLB, 28.,0.,T(2),PTM(6),S,T0,ZA,Z0,TR12)
      DD=D(3)
C       Eq. A19
      ZH28=PDM(3,3)
      ZHM28=PDM(4,3)*PDL(6,2)
      XMD=28.-XMM
      B28=DENSS(ZH28,DB28,TINF,TLB,XMD,-1.,TZ,PTM(6),S,T0,ZA,Z0,TR12)
      IF(ALT.GT.ALTL(3).OR.SW(15).EQ.0.) GO TO 17
      DM28=DENSS(ALT,B28,TINF,TLB,XMM,0.,TZ,PTM(6),S,T0,ZA,Z0,TR12)
C       Eq. A12
      D(3)=DNET(D(3),DM28,ZHM28,XMM,28.)
   17 CONTINUE
      GO TO (20,50,20,25,90,35,40,45,25,48),  J
   20 CONTINUE
C
C       **** HE DENSITY ****
C
C       Eq. A18
      G4 = SW(21)*GLOBE5(YRD,SEC,GLAT,GLONG,STL,F107A,F107,AP,PD(1,1))
      DB04 = PDM(1,1)*EXP(G4)*PD(1,1)
C       Eq. A13 - A17
      D(1)=DENSS(ALT,DB04,TINF,TLB, 4.,-.4,T(2),PTM(6),S,T0,ZA,Z0,TR12)
      DD=D(1)
      IF(ALT.GT.ALTL(1).OR.SW(15).EQ.0.) GO TO 24
C       Eq. A19
      ZH04=PDM(3,1)
      B04=DENSS(ZH04,DB04,TINF,TLB,4.-XMM,-1.4,
     $  T(2),PTM(6),S,T0,ZA,Z0,TR12)
      DM04=DENSS(ALT,B04,TINF,TLB,XMM,0.,T(2),PTM(6),S,T0,ZA,Z0,TR12)
C       Eq. A12
      ZHM04=ZHM28
      D(1)=DNET(D(1),DM04,ZHM04,XMM,4.)
C       Eq. A20b
      RL=ALOG(B28*PDM(2,1)/B04)
C       Eq. A20a
      ZC04=PDM(5,1)*PDL(1,2)
      HC04=PDM(6,1)*PDL(2,2)
      D(1)=D(1)*CCOR(ALT,RL,HC04,ZC04)
   24 CONTINUE
      IF(MASS.NE.48)   GO TO 90
   25 CONTINUE
C
C      **** O DENSITY ****
C
C       Eq. A18
      G16= SW(21)*GLOBE5(YRD,SEC,GLAT,GLONG,STL,F107A,F107,AP,PD(1,2))
      DB16 =  PDM(1,2)*EXP(G16)*PD(1,2)
C       Eq. A13 - A17
      D(2)=DENSS(ALT,DB16,TINF,TLB, 16.,0.,T(2),PTM(6),S,T0,ZA,Z0,TR12)
      DD=D(2)
      IF(ALT.GT.ALTL(2).OR.SW(15).EQ.0.) GO TO 34
C  Corrected from PDM(3,1) to PDM(3,2)  12/2/85
C       Eq. A19
      ZH16=PDM(3,2)
      B16=DENSS(ZH16,DB16,TINF,TLB,16-XMM,-1.,
     $  T(2),PTM(6),S,T0,ZA,Z0,TR12)
      DM16=DENSS(ALT,B16,TINF,TLB,XMM,0.,T(2),PTM(6),S,T0,ZA,Z0,TR12)
C       Eq. A12
      ZHM16=ZHM28
      D(2)=DNET(D(2),DM16,ZHM16,XMM,16.)
C       Eq. A20b
      RL=ALOG(B28*PDM(2,2)*ABS(PDL(17,2))/B16)
C       Eq. A20a
      HC16=PDM(6,2)*PDL(4,2)
      ZC16=PDM(5,2)*PDL(3,2)
      D(2)=D(2)*CCOR(ALT,RL,HC16,ZC16)
C       Eq. A21
      HCC16=PDM(8,2)*PDL(14,2)
      ZCC16=PDM(7,2)*PDL(13,2)
      RC16=PDM(4,2)*PDL(15,2)
      D(2)=D(2)*CCOR(ALT,RC16,HCC16,ZCC16)
   34 CONTINUE
      IF(MASS.NE.48 .AND. MASS.NE.49) GO TO 90
   35 CONTINUE
C
C       **** O2 DENSITY ****
C
C       Eq. A18
      G32= SW(21)*GLOBE5(YRD,SEC,GLAT,GLONG,STL,F107A,F107,AP,PD(1,4))
      DB32 = PDM(1,4)*EXP(G32)*PD(1,4)
C       Eq. A13 - A17
      D(4)=DENSS(ALT,DB32,TINF,TLB, 32.,0.,T(2),PTM(6),S,T0,ZA,Z0,TR12)
      IF(MASS.EQ.49) THEN
         DD=DD+2.*D(4)
      ELSE
         DD=D(4)
      ENDIF
      IF(ALT.GT.ALTL(4).OR.SW(15).EQ.0.) GO TO 39
C       Eq. A19
      ZH32=PDM(3,4)
      B32=DENSS(ZH32,DB32,TINF,TLB,32.-XMM,-1.,
     $  T(2),PTM(6),S,T0,ZA,Z0,TR12)
      DM32=DENSS(ALT,B32,TINF,TLB,XMM,0.,T(2),PTM(6),S,T0,ZA,Z0,TR12)
C       Eq. A12
      ZHM32=ZHM28
      D(4)=DNET(D(4),DM32,ZHM32,XMM,32.)
C       Eq. A20b
      RL=ALOG(B28*PDM(2,4)/B32)
C       Eq. A20a
      HC32=PDM(6,4)*PDL(8,2)
      ZC32=PDM(5,4)*PDL(7,2)
      D(4)=D(4)*CCOR(ALT,RL,HC32,ZC32)
   39 CONTINUE
      IF(MASS.NE.48)   GO TO 90
   40 CONTINUE
C
C       **** AR DENSITY ****
C
C       Eq. A18
      G40= SW(21)*GLOBE5(YRD,SEC,GLAT,GLONG,STL,F107A,F107,AP,PD(1,5))
      DB40 = PDM(1,5)*EXP(G40)*PD(1,5)
C       Eq. A13 - A17
      D(5)=DENSS(ALT,DB40,TINF,TLB, 40.,0.,T(2),PTM(6),S,T0,ZA,Z0,TR12)
      DD=D(5)
      IF(ALT.GT.ALTL(5).OR.SW(15).EQ.0.) GO TO 44
C       Eq. A19
      ZH40=PDM(3,5)
      B40=DENSS(ZH40,DB40,TINF,TLB,40.-XMM,-1.,
     $  T(2),PTM(6),S,T0,ZA,Z0,TR12)
      DM40=DENSS(ALT,B40,TINF,TLB,XMM,0.,T(2),PTM(6),S,T0,ZA,Z0,TR12)
C       Eq. A12
      ZHM40=ZHM28
      D(5)=DNET(D(5),DM40,ZHM40,XMM,40.)
C       Eq. A20b
      RL=ALOG(B28*PDM(2,5)/B40)
C       Eq. A20a
      HC40=PDM(6,5)*PDL(10,2)
      ZC40=PDM(5,5)*PDL(9,2)
      D(5)=D(5)*CCOR(ALT,RL,HC40,ZC40)
   44 CONTINUE
      IF(MASS.NE.48)   GO TO 90
   45 CONTINUE
C
C        **** HYDROGEN DENSITY ****
C
C       Eq. A18
      G1 = SW(21)*GLOBE5(YRD,SEC,GLAT,GLONG,STL,F107A,F107,AP,PD(1,6))
      DB01 = PDM(1,6)*EXP(G1)*PD(1,6)
C       Eq. A13 - A17
      D(7)=DENSS(ALT,DB01,TINF,TLB,1.,-.4,T(2),PTM(6),S,T0,ZA,Z0,TR12)
      DD=D(7)
      IF(ALT.GT.ALTL(7).OR.SW(15).EQ.0.) GO TO 47
C       Eq. A19
      ZH01=PDM(3,6)
      B01=DENSS(ZH01,DB01,TINF,TLB,1.-XMM,-1.4,
     $  T(2),PTM(6),S,T0,ZA,Z0,TR12)
      DM01=DENSS(ALT,B01,TINF,TLB,XMM,0.,T(2),PTM(6),S,T0,ZA,Z0,TR12)
C       Eq. A12
      ZHM01=ZHM28
      D(7)=DNET(D(7),DM01,ZHM01,XMM,1.)
C       Eq. A20b
      RL=ALOG(B28*PDM(2,6)*ABS(PDL(18,2))/B01)
C       Eq. A20a
      HC01=PDM(6,6)*PDL(12,2)
      ZC01=PDM(5,6)*PDL(11,2)
      D(7)=D(7)*CCOR(ALT,RL,HC01,ZC01)
C       Eq. A21
      HCC01=PDM(8,6)*PDL(20,2)
      ZCC01=PDM(7,6)*PDL(19,2)
      RC01=PDM(4,6)*PDL(21,2)
      D(7)=D(7)*CCOR(ALT,RC01,HCC01,ZCC01)
   47 CONTINUE
   48 CONTINUE
C
C        **** ATOMIC NITROGEN DENSITY ****
C
C       Eq. A18
      G14 = SW(21)*GLOBE5(YRD,SEC,GLAT,GLONG,STL,F107A,F107,AP,PD(1,7))
      DB14 = PDM(1,7)*EXP(G14)*PD(1,7)
C       Eq. A13 - A17
      D(8)=DENSS(ALT,DB14,TINF,TLB,14.,0.,T(2),PTM(6),S,T0,ZA,Z0,TR12)
      DD=D(8)
      IF(ALT.GT.ALTL(8).OR.SW(15).EQ.0.) GO TO 49
C       Eq. A19
      ZH14=PDM(3,7)
      B14=DENSS(ZH14,DB14,TINF,TLB,14.-XMM,-1.,
     $  T(2),PTM(6),S,T0,ZA,Z0,TR12)
      DM14=DENSS(ALT,B14,TINF,TLB,XMM,0.,T(2),PTM(6),S,T0,ZA,Z0,TR12)
C       Eq. A12
      ZHM14=ZHM28
      D(8)=DNET(D(8),DM14,ZHM14,XMM,14.)
C       Eq. A20b
      RL=ALOG(B28*PDM(2,7)*ABS(PDL(3,1))/B14)
C       Eq. A20a
      HC14=PDM(6,7)*PDL(2,1)
      ZC14=PDM(5,7)*PDL(1,1)
      D(8)=D(8)*CCOR(ALT,RL,HC14,ZC14)
C       Eq. A21
      HCC14=PDM(8,7)*PDL(5,1)
      ZCC14=PDM(7,7)*PDL(4,1)
      RC14=PDM(4,7)*PDL(6,1)
      D(8)=D(8)*CCOR(ALT,RC14,HCC14,ZCC14)
   49 CONTINUE
      IF(MASS.NE.48) GO TO 90
C
C       TOTAL MASS DENSITY
C
      D(6) = 1.66E-24*(4.*D(1)+16.*D(2)+28.*D(3)+32.*D(4)+40.*D(5)+
     $       D(7)+14.*D(8))
      DB48=1.66E-24*(4.*DB04+16.*DB16+28.*DB28+32.*DB32+40.*DB40+DB01+
     $        14.*DB14)
      GO TO 90
   50 DDUM  = DENSS(ALT,1., TINF,TLB,0.,0.,T(2),PTM(6),S,T0,ZA,Z0,TR12)
      GO TO 90
   90 CONTINUE
      IF(IMR.EQ.1) THEN
        DO 95 I=1,8
          D(I)=D(I)*1.E6
   95   CONTINUE
        D(6)=D(6)/1000.
      ENDIF
      RETURN
  100 FORMAT(1X,'MASS', I5, '  NOT VALID')
      ENTRY METERS(METER)
      IMR=0
      IF(METER) IMR=1
9999  CONTINUE
      END
C--------------------------------------------------------------------
      FUNCTION DENSS(ALT,DLB,TINF,TLB,XM,ALPHA,TZ,ZLB,S2,T0,ZA,Z0,TR12)
C       Calculate Temperature and Density Profiles for MSIS models
      COMMON/PARMB/GSURF,RE
      COMMON/FIT/TAF
      COMMON/LSQV/MP,II,JG,LT,QPB(50),IERR,IFUN,N,J,DV(60)
      DATA RGOpenAstro/831.4/
      ZETA(ZZ,ZL)=(ZZ-ZL)*(RE+ZL)/(RE+ZZ)
      DENSS=1.
      Z=AMAX1(ALT,ZA)
C      Eq. A4a
      ZG2=ZETA(Z,ZLB)
C      Eq. A1a
      TT=TINF-(TINF-TLB)*EXP(-S2*ZG2)
      TA=TT
      TZ=TT
      DENSS=TZ
      IF(ALT.GE.ZA) GO TO 10
C      Eq. A4b
      ZG0=ZETA(Z0,ZA)
C      Eq. A2b
      DTA=(TINF-TA)*S2*((RE+ZLB)/(RE+ZA))**2
C      Eq. A3e
      T12=T0+TR12*(TA-T0)
C      Eq. A4b
      ZG1=ZETA(ALT,ZA)
C       CALCULATE TEMPERATURE BELOW ZA
C      Eq. A3a
      DD=0.666666*ZG0*DTA/TA**2 - 3.11111*(1./TA-1./T0)+
     $ 7.11111*(1./T12-1./T0)
C      Eq. A3b
      CC=ZG0*DTA/(2.*TA*TA) - (1./TA-1./T0) - 2.*DD
C      Eq. A3c
      BB=(1./TA-1./T0) - CC - DD
C      Eq. A3d
      X=(-(ZG1-ZG0)/ZG0)
C      Eq. A1b
      X2=X*X
      TZ=1./(1./T0+BB*X2+CC*X2*X2+DD*X2*X2*X2)
      DENSS=TZ
      TAF=(T12-T0)/(TA-T0)
   10 IF(XM.EQ.0.) GO TO 50
      IF(TA.GT.0. .AND. TZ.GT.0.) GO TO 20
         WRITE(6,*)ALT,XM,TINF,TLB,T0,TA,II,JG,N,DV(J),IFUN,S2,ZG0,TZ
         TT=TLB
         TA=TLB
         TZ=TLB
   20 CONTINUE
C      CALCULATE DENSITY ABOVE ZA
C      Eq. A17a
      GLB=GSURF/(1.+ZLB/RE)**2
C      Eq. A16a
      GAMMA=XM*GLB/(S2*RGAS*TINF)
C      Eq. A13, A14a, & A15
      DENSA=DLB*(TLB/TT)**(1.+ALPHA+GAMMA)*EXP(-S2*GAMMA*ZG2)
      DENSS=DENSA
      IF(ALT.GE.ZA) GO TO 50
C      CALCULATE DENSITY BELOW ZA
C      Eq. A17b
      GLB=GSURF/(1.+ZA/RE)**2
C      Eq. A16b
      GAMM=XM*GLB*ZG0/RGAS
C      Eq. A13, A14b, & A15
      DENSS=DENSA*(TA/TZ)**(1.+ALPHA)*
     $ EXP(GAMM*((X-1)/T0+BB*(X*X2-1.)/3.+CC*(X2*X2*X-1.)/5.+
     $ DD*(X2*X2*X2*X-1.)/7.))
   50 CONTINUE
CCCCCCWRITE(6,100)CXM,ALT,ZA,TINF,TLB,S2,T0,S1,TA,TZ,DLB,DENSA,DENSS
CC100 FORMAT(' D',1P13E10.2)
      RETURN
      END
C--------------------------------------------------------------------
      FUNCTION GLOBE5(YRD,SEC,LAT,LONG,TLOC,F107A,F107,AP,P)
C       CALCULATE G(L) FUNCTION FOR MSIS-86/CIRA 1986
C       Upper Thermosphere Parameters
      REAL LAT, LONG
C!!OLD!! DIMENSION P(1),SV(25),AP(1) !!!!!!!!!!!!!!!!!!!!!!!!!!!
      DIMENSION P(*),SV(25),AP(*)
      COMMON/TTEST/TINF,GB,ROUT,T(15)/CSW/SW(25),ISW,SWC(25)
     &      /LPOLY/PLG(9,4),CTLOC,STLOC,C2TLOC,S2TLOC,C3TLOC,S3TLOC,
     &             IYR,DAY,DF,DFA,APD,APDF,APT(4)
      DATA DGTR/1.74533E-2/,DR/1.72142E-2/, XL/1000./,TLL/1000./
     &  DAYL/-1./,P14/-1000./,P18/-1000./,P32/-1000./
     &  HR/.2618/,SR/7.2722E-5/,SV/25*1./,NSW/14/,P39/-1000./
C Eq. A24d
      G0(A)=(A-4.+(P(26)-1.)*(A-4.+(EXP(-ABS(P(25))*(A-4.))-1.)/
     &  ABS(P(25))))
C Eq. A24c
      SUMEX(EX)=1.+(1.-EX**19)/(1.-EX)*EX**(.5)
C Eq. A24a
      SG0(EX)=(G0(AP(2))+(G0(AP(3))*EX+G0(AP(4))*EX*EX+G0(AP(5))*EX**3
     $ +(G0(AP(6))*EX**4+G0(AP(7))*EX**12)*(1.-EX**8)/(1.-EX)))
     $ /SUMEX(EX)
      IF(ISW.NE.64999) CALL TSELEC(SV)
      T(10) = 0.
      T(11) = 0.
      T(12) = 0.
      T(13)=0.
   10 CONTINUE
      IYR = YRD/1000.
      DAY = YRD - IYR*1000.
C Eq. A22 (remainder of code)
      IF(XL.EQ.LAT)   GO TO 15
C CALCULATE LEGENDRE POLYNOMIALS
      C = SIN(LAT*DGTR)
      S = COS(LAT*DGTR)
      C2 = C*C
      C4 = C2*C2
      S2 = S*S
      PLG(2,1) = C
      PLG(3,1) = 0.5*(3.*C2 -1.)
      PLG(4,1) = 0.5*(5.*C*C2-3.*C)
      PLG(5,1) = (35.*C4 - 30.*C2 + 3.)/8.
      PLG(6,1) = (63.*C2*C2*C - 70.*C2*C + 15.*C)/8.
      PLG(7,1) = (11.*C*PLG(6,1) - 5.*PLG(5,1))/6.
      PLG(2,2) = S
      PLG(3,2) = 3.*C*S
      PLG(4,2) = 1.5*(5.*C2-1.)*S
      PLG(5,2) = 2.5*(7.*C2*C-3.*C)*S
      PLG(6,2) = 1.875*(21.*C4 - 14.*C2 +1.)*S
      PLG(7,2) = (11.*C*PLG(6,2)-6.*PLG(5,2))/5.
      PLG(3,3) = 3.*S2
      PLG(4,3) = 15.*S2*C
      PLG(5,3) = 7.5*(7.*C2 -1.)*S2
      PLG(6,3) = 3.*C*PLG(5,3)-2.*PLG(4,3)
      PLG(7,3)=(11.*C*PLG(6,3)-7.*PLG(5,3))/4.
      PLG(8,3)=(13.*C*PLG(7,3)-8.*PLG(6,3))/5.
      PLG(4,4) = 15.*S2*S
      PLG(5,4) = 105.*S2*S*C 
      PLG(6,4)=(9.*C*PLG(5,4)-7.*PLG(4,4))/2.
      PLG(7,4)=(11.*C*PLG(6,4)-8.*PLG(5,4))/3.
      XL=LAT
   15 CONTINUE
      IF(TLL.EQ.TLOC)   GO TO 16
      STLOC = SIN(HR*TLOC)
      CTLOC = COS(HR*TLOC)
      S2TLOC = SIN(2.*HR*TLOC)
      C2TLOC = COS(2.*HR*TLOC)
      S3TLOC = SIN(3.*HR*TLOC)
      C3TLOC = COS(3.*HR*TLOC)
      TLL = TLOC
   16 CONTINUE
      IF(DAY.NE.DAYL.OR.P(14).NE.P14) CD14=COS(DR*(DAY-P(14)))
      IF(DAY.NE.DAYL.OR.P(14).NE.P14) C2D14=COS(DR*2*(DAY-P(14)))
      IF(DAY.NE.DAYL.OR.P(18).NE.P18) CD18=COS(2.*DR*(DAY-P(18)))
      IF(DAY.NE.DAYL.OR.P(32).NE.P32) CD32=COS(DR*(DAY-P(32)))
      IF(DAY.NE.DAYL.OR.P(39).NE.P39) CD39=COS(2.*DR*(DAY-P(39)))
      DAYL = DAY
      P14 = P(14)
      P18 = P(18)
      P32 = P(32)
      P39 = P(39)
C         F10.7 EFFECT
      DF = F107 - F107A
      DFA=F107A-150.
      T(1) =  P(20)*DF + P(21)*DF*DF + P(22)*DFA
     $ + P(30)*DFA**2
      F1 = 1. + (P(48)*DFA +P(20)*DF+P(21)*DF*DF)*SWC(1)
      F2 = 1. + (P(50)*DFA+P(20)*DF+P(21)*DF*DF)*SWC(1)
C        TIME INDEPENDENT
      T(2) =
     1  (P(2)*PLG(3,1) + P(3)*PLG(5,1)+P(23)*PLG(7,1))
     $ +(P(15)*PLG(3,1))*DFA*SWC(1)
     2 +P(27)*PLG(2,1)
C        SYMMETRICAL ANNUAL
      T(3) =
     1 (P(19) )*CD32
C        SYMMETRICAL SEMIANNUAL
      T(4) =
     1 (P(16)+P(17)*PLG(3,1))*CD18
C        ASYMMETRICAL ANNUAL
      T(5) =  F1*
     1  (P(10)*PLG(2,1) + P(11)*PLG(4,1))*CD14
C         ASYMMETRICAL SEMIANNUAL
      T(6) =    P(38)*PLG(2,1)*CD39
C        DIURNAL
      T71 = (P(12)*PLG(3,2) + P(36)*PLG(2,2))*CD14*SWC(5)
      T72 = (P(13)*PLG(3,2) + P(37)*PLG(2,2))*CD14*SWC(5)
      T(7) = F2*
     1 ((P(4)*PLG(2,2) + P(5)*PLG(4,2) + P(28)*PLG(6,2)
     2 + T71)*CTLOC
     4 + (P(7)*PLG(2,2) + P(8)*PLG(4,2) +P(29)*PLG(6,2)
     5 + T72)*STLOC)
C        SEMIDIURNAL
      T81 = (P(24)*PLG(4,3))*CD14*SWC(5) 
      T82 = (P(34)*PLG(4,3))*CD14*SWC(5)
      T(8) = F2*
     1 ((P(6)*PLG(3,3) + P(42)*PLG(5,3) + T81)*C2TLOC
     3 +(P(9)*PLG(3,3) + P(43)*PLG(5,3) + T82)*S2TLOC)
C        TERDIURNAL
      T(14) = F2*
     1 ((P(40)*PLG(4,4)+(P(94)*PLG(5,4)+P(47)*PLG(7,4))*CD14*SWC(5))*
     $ S3TLOC
     2 +(P(41)*PLG(4,4)+(P(95)*PLG(5,4)+P(49)*PLG(7,4))*CD14*SWC(5))*
     $ C3TLOC)
C          MAGNETIC ACTIVITY BASED ON DAILY AP
      IF(SW(9).EQ.-1. .AND. P(52).NE.0.) GO TO 30
      APD=(AP(1)-4.)
      P44=P(44)
      P45=P(45)
      IF(P44.LT.0) P44=1.E-5
      APDF = (APD+(P45-1.)*(APD+(EXP(-P44  *APD)-1.)/P44  ))
      T(9)=APDF*(P(33)+P(46)*PLG(3,1)+P(35)*PLG(5,1)+
     $ (P(101)*PLG(2,1)+P(102)*PLG(4,1)+P(103)*PLG(6,1))*CD14*SWC(5)+
     $ (P(122)*PLG(2,2)+P(123)*PLG(4,2)+P(124)*PLG(6,2))*SWC(7)*
     $ COS(HR*(TLOC-P(125))))
      GO TO 40
   30 CONTINUE
      EXP1 = EXP(-10800.*ABS(P(52))/(1.+P(139)*(45.-ABS(LAT))))
      IF(EXP1.GT..99999) EXP1=.99999
      EXP2 = EXP(-10800.*ABS(P(54)))
      IF(EXP2.GT..99999) EXP2=.99999
      IF(P(25).LT.1.E-4) P(25)=1.E-4
      APT(1)=SG0(EXP1)
      APT(3)=SG0(EXP2)
      T(9) = APT(1)*(P(51)+P(97)*PLG(3,1)+P(55)*PLG(5,1)+
     $ (P(126)*PLG(2,1)+P(127)*PLG(4,1)+P(128)*PLG(6,1))*CD14*SWC(5)+
     $ (P(129)*PLG(2,2)+P(130)*PLG(4,2)+P(131)*PLG(6,2))*SWC(7)*
     $ COS(HR*(TLOC-P(132))))
  40  CONTINUE
      IF(SW(10).EQ.0.OR.LONG.LE.-1000.) GO TO 49
C        LONGITUDINAL
      T(11)= (1.+P(90)*PLG(2,1))*(1.+P(81)*DFA*SWC(1))*
     $((P(65)*PLG(3,2)+P(66)*PLG(5,2)+P(67)*PLG(7,2)
     $ +P(104)*PLG(2,2)+P(105)*PLG(4,2)+P(106)*PLG(6,2)
     $ +SWC(5)*(P(110)*PLG(2,2)+P(111)*PLG(4,2)+P(112)*PLG(6,2))*CD14)*
     $     COS(DGTR*LONG)
     $ +(P(91)*PLG(3,2)+P(92)*PLG(5,2)+P(93)*PLG(7,2)
     $ +P(107)*PLG(2,2)+P(108)*PLG(4,2)+P(109)*PLG(6,2)
     $ +SWC(5)*(P(113)*PLG(2,2)+P(114)*PLG(4,2)+P(115)*PLG(6,2))*CD14)*
     $  SIN(DGTR*LONG))
C        UT AND MIXED UT,LONGITUDE
      T(12)=(1.+P(96)*PLG(2,1))*(1.+P(82)*DFA*SWC(1))*
     $(1.+P(120)*PLG(2,1)*SWC(5)*CD14)*
     $((P(69)*PLG(2,1)+P(70)*PLG(4,1)+P(71)*PLG(6,1))*
     $     COS(SR*(SEC-P(72))))
      T(12)=T(12)+SWC(11)*
     $ (P(77)*PLG(4,3)+P(78)*PLG(6,3)+P(79)*PLG(8,3))*
     $     COS(SR*(SEC-P(80))+2.*DGTR*LONG)*(1.+P(138)*DFA*SWC(1))
C        UT,LONGITUDE MAGNETIC ACTIVITY
      IF(SW(9).EQ.-1. .AND. P(52).NE.0.) GO TO 45
      T(13)= APDF*SWC(11)*(1.+P(121)*PLG(2,1))*
     $((P( 61)*PLG(3,2)+P( 62)*PLG(5,2)+P( 63)*PLG(7,2))*
     $     COS(DGTR*(LONG-P( 64))))
     $ +APDF*SWC(11)*SWC(5)*
     $ (P(116)*PLG(2,2)+P(117)*PLG(4,2)+P(118)*PLG(6,2))*
     $     CD14*COS(DGTR*(LONG-P(119)))
     $ + APDF*SWC(12)*
     $ (P( 84)*PLG(2,1)+P( 85)*PLG(4,1)+P( 86)*PLG(6,1))*
     $     COS(SR*(SEC-P( 76)))
      GOTO 48
   45 CONTINUE
      T(13)=APT(1)*SWC(11)*(1.+P(133)*PLG(2,1))*
     $((P(53)*PLG(3,2)+P(99)*PLG(5,2)+P(68)*PLG(7,2))*
     $     COS(DGTR*(LONG-P(98))))
     $ +APT(1)*SWC(11)*SWC(5)*
     $ (P(134)*PLG(2,2)+P(135)*PLG(4,2)+P(136)*PLG(6,2))*
     $     CD14*COS(DGTR*(LONG-P(137)))
     $ +APT(1)*SWC(12)*
     $ (P(56)*PLG(2,1)+P(57)*PLG(4,1)+P(58)*PLG(6,1))*
     $     COS(SR*(SEC-P(59)))
   48 CONTINUE
C  PARMS NOT USED: 60,83,100,140-150
   49 TINF = 0.
      IF(SW(9).EQ.-1.) TINF=P(31)
      DO 50 I = 1,NSW
   50 TINF = TINF + ABS(SW(I))*T(I)
      GLOBE5 = TINF
      RETURN
      END
C--------------------------------------------------------------------
      SUBROUTINE TSELEC(SV)
C        SET SWITCHES
      DIMENSION SV(1),SAV(25),SVV(1)
      COMMON/CSW/SW(25),ISW,SWC(25)
      DO 100 I = 1,25
        SAV(I)=SV(I)
        SW(I)=AMOD(SV(I),2.)
        IF(ABS(SV(I)).GT.0.) THEN
          SWC(I)=1.
        ELSE
          SWC(I)=0.
        ENDIF
  100 CONTINUE
      ISW=64999
      RETURN
      ENTRY TRETRV(SVV)
      DO 200 I=1,25
        SVV(I)=SAV(I)
  200 CONTINUE
      END
C--------------------------------------------------------------------
      FUNCTION GLOB5L(P)
C      LIMITED PARAMETER VERSION OF GLOBE 9/2/82
C       CALCULATE G(L) FUNCTION FOR MSIS-86/CIRA 1986
C       Lower Thermosphere Parameters
      COMMON/LPOLY/PLG(9,4),CTLOC,STLOC,C2TLOC,S2TLOC,C3TLOC,S3TLOC,
     $ IYR,DAY,DF,DFA,APD,APDF,APT(4)/CSW/SW(25),ISW,SWC(25)
C!!OLD!! DIMENSION P(1),T(15) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      DIMENSION P(*),T(15)
      DATA DR/1.72142E-2/,T/15*0./
      DATA DAYL/-1./,P7/-1000./,P9/-1000./,P11/-1000./
      IF(DAY.NE.DAYL.OR.P7.NE.P(7)) CD7=COS(DR*(DAY-P(7)))
      IF(DAY.NE.DAYL.OR.P9.NE.P(9)) CD9=COS(2.*DR*(DAY-P(9)))
      IF(DAY.NE.DAYL.OR.P11.NE.P(11)) CD11=COS(DR*(DAY-P(11)))
      DAYL=DAY
      P7=P(7)
      P9=P(9)
      P11=P(11)
C
      T(1)=P(2)*DFA
      T(2)=P(4)*PLG(3,1)
      T(3)=P(6)*CD7
      T(4)=(P(8) )*CD9
      T(5)=(P(10)*PLG(2,1)+P(22)*PLG(4,1))*CD11
      T(6)=0.
      T(7)=(P(14)*PLG(2,2)*CTLOC+P(15)*PLG(2,2)*STLOC)
      T(8)=(P(16)*PLG(3,3)+P(18)*PLG(5,3)
     $     +(P(20)*PLG(6,3))*CD11*SWC(5)
     $     )*C2TLOC
     $     +(P(17)*PLG(3,3)+P(19)*PLG(5,3)
     $     +(P(21)*PLG(6,3))*CD11*SWC(5)
     $     )*S2TLOC
      T(14)=(P(12)*PLG(4,4)*C3TLOC
     $     +P(25)*PLG(4,4)*S3TLOC)
      IF(SW(9).EQ.1)
     $ T(9)=APDF*(P(23)+P(24)*PLG(3,1)*SWC(2))
      IF(SW(9).EQ.-1)
     $ T(9)=(P(3)*APT(3)+P(5)*PLG(3,1)*APT(3)*SWC(2))
C       PARMS NOT USED: 13
      TT=0.
      DO 50 I=1,14
   50 TT=TT+ABS(SW(I))*T(I)
      GLOB5L=TT
      RETURN
      END
C--------------------------------------------------------------------
      FUNCTION DNET(DD,DM,ZHM,XMM,XM)
C       8/20/80
C       TURBOPAUSE CORRECTION FOR MSIS MODELS
C       Eq. A12b
      A=ZHM/(XMM-XM)
C       Eq. A12a
      YLOG=A*ALOG(DM/DD)
      IF(YLOG.LT.-10.) GO TO 10
      IF(YLOG.GT.10.)  GO TO 20
        DNET=DD*(1.+EXP(YLOG))**(1/A)
        GO TO 50
   10 CONTINUE
        DNET=DD
        GO TO 50
   20 CONTINUE
        DNET=DM
        GO TO 50
   50 CONTINUE
      RETURN
      END
C--------------------------------------------------------------------
      FUNCTION  CCOR(ALT, R,H1,ZH)
C        CHEMISTRY/DISSOCIATION CORRECTION FOR MSIS MODELS
C     Eq. A20a or Eq. A21
      E=(ALT-ZH)/H1
      IF(E.GT.70.) GO TO 20
      IF(E.LT.-70.) GO TO 10
        EX=EXP(E)
        CCOR=R/(1.+EX)
        GO TO 50
   10   CCOR=R
        GO TO 50
   20   CCOR=0.
        GO TO 50
   50 CONTINUE
      CCOR=EXP(CCOR)
       RETURN
      END
C--------------------------------------------------------------------
      SUBROUTINE PRMSG5
C          CIRA     11-FEB-86   
      CHARACTER ISD(3),IST(2),NAME(2),ISDATE(3),ISTIME(2)*4
      COMMON/UINR/IUMSIS,MONITO,IIEE
      COMMON/PARMB/GSURF,RE
      COMMON/PARM5/PT1(50),PT2(50),PT3(50),PA1(50),PA2(50),PA3(50),
     * PB1(50),PB2(50),PB3(50),PC1(50),PC2(50),PC3(50),
     * PD1(50),PD2(50),PD3(50),PE1(50),PE2(50),PE3(50),
     * PF1(50),PF2(50),PF3(50),PG1(50),PG2(50),PG3(50),
     * PH1(50),PH2(50),PH3(50),PI1(50)
      COMMON/LOWER5/PTM(8),PDM(8,7)/DATIME/ISDATE,ISTIME,NAME
      DATA ISD/'11-F','EB-8','6   '/,IST/'18:2','3:31'/
c
      GSURF=980.665
      RE=6356.77
      IIEE=0
2392  OPEN(UNIT=IUMSIS,FILE='MSIS86.DAT',FORM='FORMATTED',
     &	  ERR=2399,STATUS='OLD')
c2392  OPEN(UNIT=IUMSIS,FILE=
c     &'/ncf/nssdc/pub/ftp/pub/models/atmospheric/msis/msis86/msis86.dat'
c     &,FORM='FORMATTED',ERR=2399,STATUS='OLD')
      READ(IUMSIS,1290,ERR=2390) PT1,PT2,PT3,PA1,PA2,PA3,PB1,
     &	  PB2,PB3,PC1,PC2,PC3,PD1,PD2,PD3,PE1,PE2,PE3,PF1,PF2,PF3,
     &	  PG1,PG2,PG3,PH1,PH2,PH3,PI1,PTM,PDM
C
CCCCCCCCCCCCC IN PC VERSION: 1290    FORMAT(5E13.6)
C
1290  FORMAT(1X,5E13.6)
      GOTO 2395
2399  WRITE(MONITO,2396)
2396  FORMAT(' THE MSIS COEFFICIENT FILE (MSIS86.DAT) IS NOT IN',
     &  ' YOUR DIRECTORY.')
      IIEE=1
     	RETURN
2390  WRITE(MONITO,2397)
2397  FORMAT(' ERROR IN READING THE MSIS COEFFICIENT FILE (MSIS86.DAT)')
      IIEE=1
	RETURN
2395  DO 10 I=1,3
        ISDATE(I)=ISD(I)
10 	CONTINUE
      DO 20 I=1,2
        ISTIME(I)=IST(I)
20 	CONTINUE
      NAME(1)='CIRA'
      NAME(2)='-86 '
      END
C------------------------------------------------------------------
      SUBROUTINE GGM(ART,XLG,BG,XLM,BM)
C CALCULATES GEOMAGNETIC LONGITUDE (XLM) AND LATITUDE (BM) 
C FROM GEOGRAFIC LONGITUDE (XLG) AND LATITUDE (BG) FOR ART=0
C AND REVERSE FOR ART=1. ALL ANGLES IN DEGREE.
C LATITUDE:-90 TO 90. LONGITUDE:0 TO 360 EAST.
      INTEGER ART         
      FAKTOR=.0174532925                           
      ZPI=FAKTOR*360.                      
      CBG=11.4*FAKTOR                              
      CI=COS(CBG)     
      SI=SIN(CBG)
      IF(ART.EQ.0) GOTO 10                         
      CBM=COS(BM*FAKTOR)                           
      SBM=SIN(BM*FAKTOR)                           
      CLM=COS(XLM*FAKTOR)                          
      SLM=SIN(XLM*FAKTOR)
      SBG=SBM*CI-CBM*CLM*SI                     
      BG=ASIN(SBG)   
      CBG=COS(BG)     
      SLG=(CBM*SLM)/CBG  
      CLG=(SBM*SI+CBM*CLM*CI)/CBG                  
        IF(ABS(CLG).GT.1.) CLG=SIGN(1.,CLG)
        XLG=ACOS(CLG)  
      IF(SLG.LT.0.0) XLG=ZPI-ACOS(CLG)            
      BG=BG/FAKTOR    
      XLG=XLG/FAKTOR  
      XLG=XLG-69.8    
      IF(XLG.LT.0.0) XLG=XLG+360.0                 
        RETURN
10    YLG=XLG+69.8    
      CBG=COS(BG*FAKTOR)                           
      SBG=SIN(BG*FAKTOR)                           
      CLG=COS(YLG*FAKTOR)                          
      SLG=SIN(YLG*FAKTOR)                          
      SBM=SBG*CI+CBG*CLG*SI                        
      BM=ASIN(SBM)   
      CBM=COS(BM)     
      SLM=(CBG*SLG)/CBM                            
      CLM=(-SBG*SI+CBG*CLG*CI)/CBM                 
      XLM=ACOS(CLM)  
      IF(SLM.LT..0) XLM=ZPI-ACOS(CLM)             
      BM=BM/FAKTOR    
      XLM=XLM/FAKTOR  
      END             

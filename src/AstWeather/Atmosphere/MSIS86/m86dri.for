C*****************************************************************
C**************** MSIS-86 THERMOSPHERE MODEL *********************
C*****************************************************************
C****************** M86DRI.FOR, APRIL 1987 ***********************
C******************    DRIVER PROGRAM      ***********************
C*****************************************************************
C*** THIS PROGRAM PRODUCES PROFILES OF                         ***
C***      NEUTRAL TEMPERATURE                                  ***
C***      EXOSPHERIC TEMPERATURE                               ***
C***      NUMBER DENSITIES OF He, O, N2, O2, Ar, H and N       ***
C***      TOTAL MASS DENSITY                                   ***
C*****************************************************************
C*** FOR SPECIFIED                                             ***
C***      DAY OF YEAR                                          ***
C***      GEODATIC OR GEOMAGNETIC LATITUDE AND LONGITUDE       ***
C***      ALTITUDE                                             ***
C***      10.7 CM SOLAR RADIO FLUX, DAILY (FOR PREVIOUS DAY)   ***
C***                 AND 3-MONTHS AVERAGE                      ***
C***      LOCAL (APPARENT SOLAR) OR UNIVERSAL TIME             ***
C***      MAGNETIC INDEX Ap, DAILY OR HISTORY                  ***
C***      YOUR CHOICE OF VARIABLE                              ***
C*****************************************************************
C*****************************************************************
C*     --------------------ADDRESSES------------------------     *
C*     I  DR. A. E. HEDIN             DR. D. BILITZA       I     *
C*     I  GSFC CODE 614               GSFC/NSSDC CODE 633  I     *
C*     I  GREENBELT, MD 20771         GREENBELT, MD 20771  I     *
C*     I  USA                         USA                  I     *
C*     -----------------------------------------------------     *
C*****************************************************************
C*****************************************************************
C*****************************************************************
C*********       ALL ANGLES ARE IN DEGREE           **************
C*********     ALL NUMBER DENSITIES ARE IN CM-3     **************
C*********     ALL MASS DENSITIES ARE IN G/CM+3     **************
C*********       ALL ALTITUDES ARE IN KM            **************
C*********     ALL TEMPERATURES ARE IN KELVIN       **************
C*****************************************************************
C********************  OPTIONS  **********************************
C*****************************************************************
C* HOUR       IS   LOCAL TIME   OR   UNIVERSAL TIME + 25         *
C* JMAG       =0 GEOGRAFIC LATITUDE AND LONGITUDE EXPECTED       *   
C*            =1 GEOMAGNETIC LATITUDE AND LONGITUDE EXPECTED     *
C* TO TURN ON/OFF 23 PARTICULAR VARIATIONS CALL "TSELEC(SW)"     *
C*            TO SET THE 23 FLAGS SW. TO GET CURRENT VALUES OF   *
C*            SW CALL "TRETRV(SW)".                              * 
C*****************************************************************
C*****************************************************************
C*****************************************************************
      INTEGER 	EGNR,AGNR,DAYNR
      REAL 	LATI,LONGI,MLAT,MLONG
      CHARACTER ITEXT(8)*5
      CHARACTER LTEX*4
      CHARACTER IMZ(8)*4
      DIMENSION DEN(8),TEMP(2),AP(16),XVAR(8),ISWI(25),SWI(25),
     &  	OUTF(11,30),PARMAX(8),PARMIN(8)
      COMMON/UINR/IUMSIS,MONITO,IIEE
      COMMON/GTS3C/DL(16)
      LOGICAL 	UNTI,UTLT,NOTBEG,OUT1
      DATA IMZ/'GEOD','GEOD',' DAY',' MON','    ','L.T.','    ',
     &  '    '/,ITEXT/' LATI',' LONG','F10.7','F10.7',' DAY ',
     &  ' HOUR',' H/KM','  AP '/,LATI,LONGI,HEIGHT,F107,F107M,
     &  DAYNR,HOUR,AP(1),IVAR,BVAR,EVAR,SVAR/45.1,293.1,100,
     &  100,100,231,12.5,0,7,100,1000,100/,ISWI/25*1/,SWI/25*1.0/
      DATA PARMIN/-90.,-360.,50.,60.,1.,0.,85.,0./,PARMAX/90.,
     &  360.,400.,400.,366.,49.,3000.,500./,JAGNR/0/
      UMR=ATAN(1.0)*4./180.
      ALOG2=ALOG(2.)
      ISTART=1
      LTEX='(LT)'
      MAX1=1
      MIN0=0
C
C FIRST SPECIFY YOUR COMPUTERS CHANNEL NUMBERS ....................
C EGNR=INPUT, MONITO=MONITOR, AGNR=OUTPUT .........................
C IUMSIS=UNIT NUMBER FOR COEFFICIENT INPUT ........................
C IUOUT=UNIT NUMBER FOR OUTPUTFILE OUTPUT.MSI .....................
C
      EGNR=5
      MONITO=6
      IUMSIS=12
      IUOUT=16
      WRITE(MONITO,5100)
5100  FORMAT(1X/////4X,'************* MSIS THERMOSPHERE MODEL',
     & ' 1986 **************'/1X,63('*')/
     & ' THIS PROGRAM ALLOWS YOU TO PRODUCE MSIS-PROFILES IN LATITUDE,'/
     & ' LONGITUDE,SOLAR ACTIVITY,DAY,TIME,ALTITUDE OR MAGNETIC',
     & ' ACTIVITY.'/1X,63('*'))
      WRITE(MONITO,5111)
5111  FORMAT('  In each of the following windows you will be asked ',
     & 'to enter'/'  one or more values, defining the conditions for ',
     & 'your MSIS'/'  tables.'/'  In each window the current value(s) ',
     & 'is (are) shown in the right'/
     & '  upper corner (#..#). You can choose the current value(s) by'/
     & '  entering / at the prompt'/'  If you enter a wrong character',
     & ' or a value outside the allowed'/'  parameter range, the ',
     & 'program will ask you for a new entry.'/'  After your tables',
     & ' are displayed, you can change any parameter'/'  you wish to ',
     & 'change and create new tables.'/'  You can leave the program',
     & ' at any point by entering Ctrl Z.'/1X,63('*'))
      NOTBEG=.FALSE.
      GOTO 5508
3293  DLATI=LATI
      DLONG=LONGI
      IF(JMAG.EQ.1) THEN
	DLATI=MLAT
	DLONG=MLONG
      ENDIF
      DHOU=HOUR
      LTEX='(LT)'
      IF(UNTI) THEN
	DHOU=UT
	LTEX='(UT)'
      ENDIF
      WRITE(MONITO,5602) DLATI,JMAG,DLONG,ITEXT(IVAR),HEIGHT,BVAR,
     & EVAR,SVAR,F107,F107M,DAYNR,LTEX,DHOU,AP(1)
      ISTART=ISTART+1
5602  FORMAT(1X//' **** WHICH PARAMETER DO YOU WANT TO ',
     & 'CHANGE?'/1X,70('-')/' 0  NO FURTHER CHANGES, CALCULATE PRO',
     & 'FILE'/' 1  LATITUDE  #',F5.1,'#',8X,'7  GEOD.(0) OR GEOM.(',
     & '1)    #',I1,'#'/' 2  LONGITUDE #',F5.1,'#',8X,'8  SELECTION',
     & ' OF VARIABLE   #',A5,'#'/' 3  ALTITUDE  #',F6.1,'#',7X,'9  ',
     & 'VARIABLE RANGE  #',3F8.1,'#'/' 4  F10.7     #',F5.1,'#',7X,
     & '10  F10.7M',18X,'#',F5.1,'#'/' 5  DAY',7X,'#',I3,'#',9X,'11',
     & '  ON/OFF FOR PARTICULAR VARIATIONS'/' 6  HOUR ',A4,' #',F5.1,
     & '#',7X,'12  DISPLAY OR STORE'/28X,'13  AP',23X,'#',F5.1,'#'/
     & 1X,70('-')/' ENTER NUMBER')
      MAXI=13   
8603  READ(EGNR,*,ERR=8600,END=2978) ISWIT
      IF((ISWIT.GE.MIN0).AND.(ISWIT.LE.MAXI)) GOTO 8601
8600  WRITE(MONITO,8602) MIN0,MAXI
8602  FORMAT(' Your input is outside the value range:',I2,' to ',I3,
     &	', try again')
      GOTO 8603
8601  GOTO (5300,3329,3339,3331,3500,5501,5502,3332,5503,5504,
     &    5701,5505,5508,6780) ISWIT+1
5508  WRITE(MONITO,5123) JAGNR
5123  FORMAT(/' DO YOU WANT YOUR MSIS-PROFILES',27X,'#',I1,'#'/5X,
     & 'DISPLAYED ON YOUR MONITOR:  ENTER  0  AT PROMPT'/5X,
     & 'STORED IN FILE OUTPUT.MSI:  ENTER  1  AT PROMPT'/5X,
     & 'DISPLAYED AND STORED:       ENTER  2  AT PROMPT')
      MAXI=2
      WRITE(MONITO,8630)
8630  FORMAT(1X,60('-')/' enter /, to continue with current value(s);',
     &   ' Ctrl Z, to exit')
8604  READ(EGNR,*,ERR=8605,END=2978) JAGNR
      IF((JAGNR.GE.MIN0).AND.(JAGNR.LE.MAXI)) GOTO 8606
8605  WRITE(MONITO,8602) MIN0,MAXI
      GOTO 8604
8606  IVARNR=0
      AGNR=MONITO
      IF(JAGNR.GT.0) THEN
        OPEN(UNIT=IUOUT,FILE='OUTPUT.MSI',STATUS='NEW',FORM='FORMATTED')
        IF(JAGNR.EQ.1) AGNR=IUOUT
      ENDIF
      IF(NOTBEG) GOTO 3293
5503  WRITE(MONITO,5040) IVAR
5040  FORMAT(1X//' SELECT YOUR VARIABLE:',36X,'#',I1,'#'//34X, 
     &  'Geodetic or geomagnetic coordinates'/34X,'are assumed dep',
     &  'ending on your later'/' 1  LATITUDE    5  DAY OF YEAR',4X,
     &  'selection of JMAG.'/' 2  LONGITUDE   6  HOUR',11X,'Varia',
     &  'tion with Universal Time is chosen'/' 3  F10.7',7X,'7  ',
     &  'ALTITUDE',7X,'by adding "25" to your BEGIN hour in '/' 4 ',
     &  ' F10.7M',6X,'8  AP',13X,'the next window.'/34X,'If you se',
     &  'lect F10.7M, F10.7 = F10.7M'/34X,'will be assumed.')
      WRITE(MONITO,8630)
      MAXI=8
      MINI=1   
8610  READ(EGNR,*,ERR=8611,END=2978) IVAR
      IF((IVAR.GE.MINI).AND.(IVAR.LE.MAXI)) GOTO 5504
8611  WRITE(MONITO,8602) MINI,MAXI
      GOTO 8610
5504  WRITE(MONITO,5044) BVAR,EVAR,SVAR
5044  FORMAT(1X//' CHOOSE YOUR VARIABLE RANGE:',5X,' BEGIN, END, ',
     & 'STEPWIDTH ?'/32X,'#',F8.1,',',F8.1,',',F8.1,'#'/)
      IF(IVAR.EQ.6) WRITE(MONITO,8741)
8741  FORMAT(' !! if your variable is HOUR and you add "25" to your',
     & ' BEGIN '/' !! hour UNIVERSAL TIME is assumed, otherwise ',
     & 'LOCAL TIME')
      WRITE(MONITO,8630)
      XMAX=PARMAX(IVAR)
      XMIN=PARMIN(IVAR)
9630  READ(EGNR,*,ERR=8631,END=2978) BVAR,EVAR,SVAR
      IF((BVAR.GE.XMIN).AND.(EVAR.LE.XMAX)) GOTO 8632
8631  WRITE(MONITO,8633) XMIN,XMAX
8633  FORMAT(' Your input is outside the value range:',F7.1,' to',F7.1,
     &	', try again')
      GOTO 9630
8632  IF(IVAR.NE.6) GOTO 2929
        IMZ(6)='L.T.'
	UNTI=.FALSE.
      IF(BVAR.GT.24.1) THEN
	UNTI=.TRUE.
        IMZ(6)='U.T.'
	BVAR=BVAR-25.
      ENDIF
2929  LANZ=INT((EVAR-BVAR)/SVAR)+1
      IF(LANZ.GT.30) THEN
         WRITE(MONITO,6013)
6013     FORMAT(' The maximum table size is 30 rows. Please choose a'/
     &      ' smaller range or larger stepsize.')
         GOTO 9630
         ENDIF
      IF(NOTBEG) GOTO 3293
3332  WRITE(MONITO,6006) JMAG
6006  FORMAT(1X//1X,'JMAG ?',50X,'#',I1,'#'//' !! "0" geodetic',
     &  ' longitude and latitude'/' !! "1"    geomagnetic   ... ')
      WRITE(MONITO,8630)
8640  READ(EGNR,*,ERR=8641,END=2978) JMAG
      IF((JMAG.GE.MIN0).AND.(JMAG.LE.MAX1)) GOTO 8642
8641  WRITE(MONITO,8602) MIN0,MAX1
      GOTO 8640
8642  CONTINUE
	IMZ(1)='GEOD'
	IMZ(2)='GEOD'
      IF(JMAG.EQ.1) THEN
	IMZ(1)='GEOM'
	IMZ(2)='GEOM'
      ENDIF
      IF(NOTBEG) GOTO 3293
      IVARNR=IVARNR+1
      IF(IVARNR.EQ.IVAR) GOTO 7339
3329  DLATI=LATI
      IF(JMAG.EQ.1) DLATI=MLAT
      WRITE(MONITO,5000) IMZ(1),DLATI
5000  FORMAT(1X//1X,A4,' LATITUDE ?   !NORTH!    [DEGREE,DECIMAL]',
     & 8X,'#',F5.1,'#')
      WRITE(MONITO,8630)
      XMAX=PARMAX(1)
      XMIN=PARMIN(1)
8650  READ(EGNR,*,ERR=8651,END=2978) DLATI
      IF((DLATI.GE.XMIN).AND.(DLATI.LE.XMAX)) GOTO 8652
8651  WRITE(MONITO,8633) XMIN,XMAX
      GOTO 8650
8652  IF(JMAG.EQ.0) THEN
	LATI=DLATI
      ELSE
	MLAT=DLATI
      ENDIF
      IF(NOTBEG) GOTO 3293
7339  IVARNR=IVARNR+1
      IF(IVARNR.EQ.IVAR) GOTO 7500
3339  DLONG=LONGI
      IF(JMAG.EQ.1) DLONG=MLONG
      WRITE(MONITO,6001) IMZ(2),DLONG
6001  FORMAT(1X//1X,A4,' LONGITUDE ?   !EAST!    [DEGREE,DECIMAL]',
     & 8X,'#',F5.1,'#')
      WRITE(MONITO,8630)
      XMAX=PARMAX(2)
      XMIN=PARMIN(2)
8653  READ(EGNR,*,ERR=8654,END=2978) DLONG
      IF((DLONG.GE.XMIN).AND.(DLONG.LE.XMAX)) GOTO 8655
8654  WRITE(MONITO,8633) XMIN,XMAX
      GOTO 8653
8655  IF(JMAG.EQ.0) THEN
	LONGI=DLONG
      ELSE
	MLONG=DLONG
      ENDIF
      IF(NOTBEG) GOTO 3293
7500  IVARNR=IVARNR+1
      IF(IVARNR.EQ.IVAR) GOTO 7701
3500  WRITE(MONITO,6003) F107
6003  FORMAT(1X//1X,'10.7CM SOLAR RADIO FLUX ?    * PREVIOUS DAY *',
     &  8X,'#',F5.1,'#')
      WRITE(MONITO,8630)
      XMAX=PARMAX(3)
      XMIN=PARMIN(3)
8656  READ(EGNR,*,ERR=8657,END=2978) F107
      IF((F107.GE.XMIN).AND.(F107.LE.XMAX)) GOTO 8658
8657  WRITE(MONITO,8633) XMIN,XMAX
      GOTO 8656
8658  IF(NOTBEG) GOTO 3293
7701  IVARNR=IVARNR+1
      IF(IVARNR.EQ.IVAR) GOTO 7501
5701  WRITE(MONITO,6913) F107M
6913  FORMAT(1X//1X,'10.7CM SOLAR RADIO FLUX ?   * MONTHLY AVERAGE *',
     &  6X,'#',F5.1,'#')
      WRITE(MONITO,8630)
      XMAX=PARMAX(4)
      XMIN=PARMIN(4)
8659  READ(EGNR,*,ERR=8660,END=2978) F107M
      IF((F107M.GE.XMIN).AND.(F107M.LE.XMAX)) GOTO 8661
8660  WRITE(MONITO,8633) XMIN,XMAX
      GOTO 8659
8661  IF(NOTBEG) GOTO 3293
7501  IVARNR=IVARNR+1
      IF(IVARNR.EQ.IVAR) GOTO 7502
5501  WRITE(MONITO,6004) DAYNR
6004  FORMAT(1X//1X,'DAY OF YEAR ?',8X,'* DDD *',27X,'#',I3,'#')
      WRITE(MONITO,8630)
      MAXI=366
8690  READ(EGNR,*,ERR=8691,END=2978) DAYNR
      IF((DAYNR.GE.MAX1).AND.(DAYNR.LE.MAXI)) GOTO 8692
8691  WRITE(MONITO,8602) MAX1,MAXI
      GOTO 8690
8692  IF(NOTBEG) GOTO 3293
7502  IVARNR=IVARNR+1
      IF(IVARNR.EQ.IVAR) GOTO 7331
5502  DHOU=HOUR
      IF(UNTI) DHOU=UT
      WRITE(MONITO,6005) DHOU,LTEX
6005  FORMAT(1X//1X,'LOCAL TIME ?',8X,'[HOURS, DECIMAL]',14X,'#',
     &  F4.1,A4,'#'//' !! Universal Time (UT) is assumed, if you enter',
     &  ' UT+25.'/' !! Enter -1, if you like to provide LOCAL TIME',
     &  ' as well as UNIVERSAL TIME')
      WRITE(MONITO,8630)
      XMAX=PARMAX(6)
      XMIN=-1.0
8665  READ(EGNR,*,ERR=8667,END=2978) DHOUR
      IF((DHOUR.GE.XMIN).AND.(DHOUR.LE.XMAX)) GOTO 8668
8667  WRITE(MONITO,8633) XMIN,XMAX
      GOTO 8665
8668  UNTI=.FALSE.
      IF(DHOUR.GT.24.1) THEN
	UNTI=.TRUE.
	UT=DHOUR-25.
      ELSE
	HOUR=DHOUR
      ENDIF
      UTLT=.FALSE.
      IF(HOUR.LT.0.0) THEN
        WRITE(MONITO,6605)
6605    FORMAT(//' LOCAL TIME, UNIVERSAL TIME ?    [HOURS, DECIMAL]'
     &      /1X,60('-'))
	XMAX=24.0
	XMIN=0.0
8670	READ(EGNR,*,ERR=8671,END=2978) HOUR,UT
	IF((HOUR.LT.XMIN).OR.(HOUR.GT.XMAX)) GOTO 8671
	IF((UT.GE.XMIN).AND.(UT.LE.XMAX)) GOTO 8672
8671	WRITE(MONITO,8633) XMIN,XMAX
     	GOTO 8670
8672  	UTLT=.TRUE.
      ENDIF
      IF(NOTBEG) GOTO 3293
7331  IVARNR=IVARNR+1
      IF(IVARNR.EQ.IVAR) GOTO 5551
3331  WRITE(MONITO,6002) HEIGHT
6002  FORMAT(1X//1X,'ALTITUDE ?    [KM]',34X,'#',F6.1,'#')
      WRITE(MONITO,8630)
      XMAX=PARMAX(7)
      XMIN=PARMIN(7)
8675  READ(EGNR,*,ERR=8676,END=2978) HEIGHT
      IF((HEIGHT.GE.XMIN).AND.(HEIGHT.LE.XMAX)) GOTO 8677
8676  WRITE(MONITO,8633) XMIN,XMAX
      GOTO 8675
8677  IF(NOTBEG) GOTO 3293
5551  IVARNR=IVARNR+1
      IF(IVARNR.EQ.IVAR) GOTO 5505
6780  WRITE(MONITO,7732) AP(1)
7732  FORMAT(1X//1X,'AP-INDEX ?    * MAGNETIC ACTIVITY *',18X,'#',
     & F5.1,'#'//' !! "-1"   enter detailed AP history for 59 hours'
     & /' !!        prior to current time')
      WRITE(MONITO,8630)
      XMAX=PARMAX(8)
      XMIN=-1.
8680  READ(EGNR,*,ERR=8681,END=2978) AP1
      IF((AP1.GE.XMIN).AND.(AP1.LE.XMAX)) GOTO 8682
8681  WRITE(MONITO,8633) XMIN,XMAX
      GOTO 8680
8682  IF(AP1.GT.-1.0) THEN
	AP(1)=AP1
	GOTO 3999
	ENDIF
      ISWI(9)=-1
3813    WRITE(MONITO,6620) (AP(J),J=1,7)
6620    FORMAT(' 1  DAILY AP',45X,'#',F5.1,'#'/' 2  3-H(OUR)-AP INDEX',
     &   ' FOR CURRENT TIME',19X,'#',F5.1,'#'/' 3  3-H-AP INDEX FOR ',
     &   '3 HRS BEFORE CURRENT TIME',11X,'#',F5.1,'#'/' 4  3-H-AP ',
     &   'INDEX FOR 6 HRS BEFORE CURRENT TIME',11X,'#',F5.1,'#'/
     &   ' 5  3-H-AP INDEX FOR 9 HRS BEFORE CURRENT TIME',11X,'#',F5.1,
     &   '#'/' 6  AVERAGE OF 3-H-AP INDICIES FROM 12 TO 33 HRS ', 
     &   'BEFORE  #',F5.1,'#'/' 7  AVERAGE OF 3-H-AP INDICIES FROM ',
     &   '36 TO 59 HRS BEFORE  #',F5.1,'#'/1X,70('-')/' ENTER:',
     &   ' NUMBER, AP-VALUE    (0,0 TO CONTINUE)')
      MAXI=7
      XMIN=0.
8694  READ(EGNR,*,ERR=8695,END=2978) NRAP,APN
      IF((NRAP.GE.MIN0).AND.(NRAP.LE.MAXI)) GOTO 8696
8695  WRITE(MONITO,8602) MIN0,MAXI
      GOTO 8694
8696  IF((APN.GE.XMIN).AND.(APN.LE.XMAX)) GOTO 8779
      WRITE(MONITO,8633) XMIN,XMAX
      GOTO 8694
8779  IF(NRAP.GT.0) THEN
	AP(NRAP)=APN
	GOTO 3813
	ENDIF
3999  IF(NOTBEG) GOTO 3293
5505  IF(NOTBEG) CALL TRETRV(SWI)
        IF(ISWI(9).LT.0) SWI(9)=ISWI(9)*1.0
        DO 3919 J=1,25
3919    ISWI(J)=INT(SWI(J))
5521  WRITE(MONITO,5030) (ISWI(J),J=1,23) 
5030  FORMAT(1X//' SELECTION OF PARTICULAR VARIATIONS:  "0"  OFF   ',
     & '"1"  ON'/5X,'"2"  MAIN EFFECT OFF, BUT CROSS TERMS ON',5X,
     & '"-1"  SPECIAL'/1X,70('-')/
     & ' 1  F10.7 EFFECT ON MEAN   #',I1,'#',9X,'2  TIME',
     & ' INDEPENDENT        #',I1,'#'/' 3  SYMMETRICAL ANNUAL     #',
     & I1,'#',9X,'4  SYMMETRICAL SEMI-ANNUAL #',I1,'#'/' 5  ASYMMETR',
     & 'ICAL ANNUAL    #',I1,'#',9X,'6  ASYMMETRICAL SEMI-ANNUAL#',I1,
     & '#'/' 7  DIURNAL',16X,'#',I1,'#',9X,'8  SEMI-DIURNAL',12X,'#',
     & I1,'#'/' 9  DAILY AP (-1 HISTORY) #',I2,'#',8X,'10  ALL UT/LO',
     & 'NG EFFECTS     #',I1,'#'/' 11 LONGITUDINAL',11X,'#',I1,'#',8X,
     & '12  UT AND MIXED UT/LONG    #',I1,'#'/' 13 MIXED AP/UT/LONG',
     & 7X,'#',I1,'#',8X,'14  TER-DIURNAL',13X,'#',I1,'#'/' 15 DEPART',
     & 'URES FROM DIFF. EQUIL. #',I1,'#  16  ALL TINF VARIATION',6X,
     & '#',I1,'#'/' 17 ALL TLB VARIATION      #',I1,'#',8X,'18  ALL',
     & ' T0 VARIATION        #',I1,'#'/' 19 ALL S VARIATION        #',
     & I1,'#',8X,'20  ALL Z0 VARIATION        #',I1,'#'/' 21 ALL NLB',
     & ' VARIATION      #',I1,'#',8X,'22  ALL TR12 VARIATION      #',
     & I1,'#'/' 23 TURBO SCALE HEIGHT VARIATION #',I1,'#'/1X,70('-')/
     & ' ENTER: NUMBER, NEW FLAG     OR: "0,0" TO CONTINUE')
      MAXI=23
      MINI=-1
      MAX2=2   
8700  READ(EGNR,*,ERR=8701,END=2978) ISELEC,IVALUE
      IF((ISELEC.GE.MIN0).AND.(ISELEC.LE.MAXI)) GOTO 8702
8701  WRITE(MONITO,8602) MIN0,MAXI
      GOTO 8700
8702  IF((IVALUE.LT.MINI).OR.(IVALUE.GT.MAX2)) THEN
	WRITE(MONITO,8602) MINI,MAX2
	GOTO 8700
	ENDIF
      IF(ISELEC.GT.0) THEN
        ISWI(ISELEC)=IVALUE
        SWI(ISELEC)=IVALUE*1.0
        GOTO 5521
      ENDIF
      CALL TSELEC(SWI)
      IF(NOTBEG) GOTO 3293
5300  NOTBEG=.TRUE.
      WRITE(AGNR,8193) ITEXT(IVAR),IMZ(IVAR)
      IF(JAGNR.EQ.2) WRITE(IUOUT,8193) ITEXT(IVAR),IMZ(IVAR)
8193  FORMAT(//////////3X,A5,8X,'NUMBER DENSITIES/CM-3     MASS',
     &  ' DENSITY  TEMPERATURE/K'/4X,A4,4X,'   O         N2    ',
     &  '     O2       G*CM-3       TN   TEXO')
	XVAR(1)=LATI
	XVAR(2)=LONGI
	IF(JMAG.EQ.1) THEN
	  XVAR(1)=MLAT
	  XVAR(2)=MLONG
	ENDIF
        XVAR(3)=F107
        XVAR(4)=F107M
        XVAR(5)=DAYNR
        XVAR(6)=HOUR
        IF(UNTI) XVAR(6)=UT
        XVAR(7)=HEIGHT
        XVAR(8)=AP(1)
        LFD=0
        XVAR(IVAR)=BVAR-SVAR
2123    XVAR(IVAR)=XVAR(IVAR)+SVAR
        LFD=LFD+1
        IF(JMAG.EQ.1) THEN
           MLAT=XVAR(1)
           MLONG=XVAR(2)
        ELSE
           LATI=XVAR(1)
           LONGI=XVAR(2)
        ENDIF
        F107=XVAR(3)
        F107M=XVAR(4)
        IF(IVAR.EQ.4) F107=F107M
        DAYNR=XVAR(5)
        IF(UNTI) THEN
	  UT=XVAR(6)
 	ELSE
	  HOUR=XVAR(6)
	ENDIF
        HEIGHT=XVAR(7)
        AP(1)=XVAR(8)
C
C MSIS CALCULATION
C
        IF((LFD.EQ.1).OR.(IVAR.LT.3))
     &          CALL GGM(JMAG,LONGI,LATI,MLONG,MLAT)
        IF(UTLT) GOTO 3820
        IF(UNTI) THEN
          HOUR=UT+LONGI/15.
          IF(HOUR.GT.24.0) HOUR=HOUR-24.0
        ELSE
	  UT=HOUR-LONGI/15.
          IF(UT.LT.0.0) UT=UT+24.
        ENDIF
3820    UTSEC=UT*3600.0
        CALL GTS5(DAYNR,UTSEC,HEIGHT,LATI,LONGI,HOUR,F107M,F107,
     &          AP(1),48,DEN,TEMP)
      XCOR=XVAR(IVAR)
      OUTF(1,LFD)=XCOR
      DO 2918 LL=1,8
2918  OUTF(LL+1,LFD)=DEN(LL)
      OUTF(10,LFD)=TEMP(1)
      OUTF(11,LFD)=TEMP(2)
      WRITE(AGNR,7117) XCOR,DEN(2),DEN(3),DEN(4),DEN(6),
     &  INT(TEMP(2)+.5),INT(TEMP(1)+.5)
      IF(JAGNR.EQ.2) WRITE(IUOUT,7117) XCOR,DEN(2),DEN(3),DEN(4),
     &  DEN(6),INT(TEMP(2)+.5),INT(TEMP(1)+.5)
7117  FORMAT(1X,F7.1,1X,3(1X,1PE10.3),2X,1PE9.3,2X,2I6)
      IF(XCOR.LT.EVAR) GOTO 2123
2289  WRITE(AGNR,2193) LATI,LONGI,HEIGHT,F107,DAYNR,HOUR
     &   ,AP(1),MLAT,MLONG,F107M,UT
      IF(JAGNR.EQ.2) WRITE(IUOUT,2193) LATI,LONGI,HEIGHT,F107,
     &   DAYNR,HOUR,AP(1),MLAT,MLONG,F107M,UT
2193  FORMAT(1X,74('-')/' LATI/LONG=',F5.1,'/',F5.1,'  H=',F6.1,
     &  '  F10.7 =',F5.1,'  DAY:',I3,'  LT:',F4.1,'  AP=',F5.1/
     &  ' MLAT/MLON=',F5.1,'/',F5.1,12X,'F10.7M=',F5.1,11X,
     &  'UT:',F4.1/1X,74('-'))
        OUT1=.TRUE.
3918  WRITE(MONITO,5600)
5600  FORMAT(1X/' **** DO YOU WANT TO CONTINUE?'/1X,60('-')/
     &  ' "0"  QUIT AND EXIT        "1"  NEW PARAMETERS'/
     &  ' "2"  OTHER OUTPUT WINDOW'/1X,60('-'))
      MAXI=2
8750  READ(EGNR,*,ERR=8751,END=2978) IALL
      IF((IALL.GE.MIN0).AND.(IALL.LE.MAXI)) GOTO 8752
8751  WRITE(MONITO,8602) MIN0,MAXI
      GOTO 8750
8752  IF(IALL.EQ.1) GOTO 3293
      IF(IALL.EQ.0) GOTO 2978
      IF(OUT1) GOTO 1938
        WRITE(AGNR,8193) ITEXT(IVAR),IMZ(IVAR)
        IF(JAGNR.EQ.2) WRITE(IUOUT,8193) ITEXT(IVAR),IMZ(IVAR)
      DO 2891 I=1,LFD
      WRITE(AGNR,7117) OUTF(1,I),OUTF(3,I),OUTF(4,I),OUTF(5,I),
     &  OUTF(7,I),INT(OUTF(11,I)+.5),INT(OUTF(10,I)+.5)
      IF(JAGNR.EQ.2) WRITE(IUOUT,7117) OUTF(1,I),OUTF(3,I),
     &	OUTF(4,I),OUTF(5,I),OUTF(7,I),INT(OUTF(11,I)+.5),
     &	INT(OUTF(10,I)+.5)
2891  CONTINUE
      WRITE(AGNR,2193) LATI,LONGI,HEIGHT,F107,DAYNR,HOUR,
     &  AP(1),MLAT,MLONG,F107M,UT
      IF(JAGNR.EQ.2) WRITE(IUOUT,2193) LATI,LONGI,HEIGHT,F107,
     &  DAYNR,HOUR,AP(1),MLAT,MLONG,F107M,UT
      OUT1=.TRUE.
      GOTO 3918
1938  WRITE(AGNR,8195) ITEXT(IVAR),IMZ(IVAR)
      IF(JAGNR.EQ.2) WRITE(IUOUT,8195) ITEXT(IVAR),IMZ(IVAR)
8195  FORMAT(//////////3X,A5,13X,'NUMBER DENSITIES/CM-3',12X,
     &  'TEMPERATURE/K'/4X,A4,7X,'HE',9X,'AR',9X,'H',
     &  10X,'N',9X,'TN')
      DO 2899 I=1,LANZ
      IF(JAGNR.EQ.2) WRITE(IUOUT,7119) OUTF(1,I),OUTF(2,I),
     &  OUTF(6,I),OUTF(8,I),OUTF(9,I),INT(OUTF(11,I)+.5)
      WRITE(AGNR,7119) OUTF(1,I),OUTF(2,I),OUTF(6,I),
     &  OUTF(8,I),OUTF(9,I),INT(OUTF(11,I)+.5)
2899  CONTINUE
7119  FORMAT(1X,F7.1,1X,4(1X,1PE10.3),2X,I6)
      WRITE(AGNR,2193) LATI,LONGI,HEIGHT,F107,DAYNR,
     &   HOUR,AP(1),MLAT,MLONG,F107M,UT
      IF(JAGNR.EQ.2) WRITE(IUOUT,2193) LATI,LONGI,HEIGHT,F107,DAYNR,
     &   HOUR,AP(1),MLAT,MLONG,F107M,UT
      OUT1=.FALSE.
      GOTO 3918
2978  STOP
      END

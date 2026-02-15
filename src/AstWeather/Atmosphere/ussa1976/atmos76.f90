!+
MODULE Atmosphere1976
! ---------------------------------------------------------------------------
! PURPOSE - Compute properties of the U.S. Standard Atmosphere 1976
! AUTHORS - Steven S. Pietrobon. Small World 
!           Ralph L. Carmichael, Public Domain Aeronautical Software
! REFERENCE - "The U.S. Standard Atmosphere, 1976" published by the 
!    Government Printing Office. Also filed as NASA TM X-74335.
!    NASA SP-398 is another useful reference.
!
!     REVISION HISTORY
!   DATE  VERS PERSON  STATEMENT OF CHANGES
! 28Feb95  0.1   RLC   Assembled several old codes
!  1Aug00  0.2   RLC   Copied from old Tables76
! 23Aug00  0.3   RLC   Added NitrogenNumber using QUANC8 (later removed)
! 24Aug00  0.4   RLC   Added KineticTemperature
! 30Aug00  0.5   RLC   Corrected numerous errors
! 30Dec00  0.6   RLC   Adapted UpperAtmosphere from Pietrobon's Unofficial
!                        Australian Standard Atmosphere
! 30Jun18  0.7   RLC   Use tables of sigma and delta rather than p and rho 
! 19Jul18  0.8   RLC   Added subroutine TransportRatios
! 24Jul18  0.9   RLC   Added function MolecularWeight 
! 11Mar22  0.91  RLC   "final" cleanup
!-------------------------------------------------------------------------------
IMPLICIT NONE
  CHARACTER(LEN=*),PUBLIC,PARAMETER:: ATM76_VERSION = "0.91 (11 March 2022)"
  REAL,PRIVATE,PARAMETER:: PI = 3.14159265358979323846264338
!
! various choices for the radius in kilometers of the ideal spherical earth...
! The value of POLAR_RADIUS is used, to agree with the reference document.  
! The value 6356.766 was chosen as best in 1976. Today we use 6356.7523 as
! best measurement, but here we stick with the value used in computing the 
! tables in the reference document.
  REAL,PARAMETER,PRIVATE:: POLAR_RADIUS = 6356.766  
  REAL,PARAMETER,PRIVATE:: EQUATORIAL_RADIUS = 6378.1370
  REAL,PARAMETER,PRIVATE:: RADIUS_AT_45DEG_LAT = 6367.4895
  REAL,PARAMETER,PRIVATE:: AUTHALIC_RADIUS = 6371.0012               ! same area 
  REAL,PARAMETER,PRIVATE:: VOLUMETRIC_RADIUS = 6371.0008           ! same volume
  REAL,PARAMETER:: REARTH = POLAR_RADIUS              ! radius of the Earth (km)

! Some physical constants and properties of air
  REAL,PARAMETER:: GZERO = 9.80665         !  sea level accel. of gravity, m/s^2
  REAL,PARAMETER:: MOLWT_ZERO = 28.9644    ! molecular weight of air at sealevel
  REAL,PARAMETER:: RSTAR = 8314.32          ! perfect gas constant, N-m/(kmol-K)
  REAL,PARAMETER:: GMR=1000*GZERO*MOLWT_ZERO/RSTAR ! hydrostatic constant, kelvins/km
! NOTE - Observe the factor of 1000 in computing GMR. Without it, GMR would have 
!   units of kelvins per meter. But the temperature gradient in LowerAtmosphere
!   is given in kelvins per kilometer.   GMR=34.163195 kelvins/km

  REAL,PARAMETER:: BETAVISC = 1.458E-6       ! viscosity term, N s/(sq.m sqrt(K)
  REAL,PARAMETER:: SUTH = 110.4                       ! Sutherland's constant, K
  REAL,PARAMETER:: AVOGADRO =  6.022169E26           ! 1/kmol, Avogadro constant
  REAL,PARAMETER:: BOLTZMANN = 1.380622E-23           ! Nm/K, Boltzmann constant
  
! Some sea level values of various quantities  
  REAL,PARAMETER:: TZERO = 288.15                   ! temperature at sealevel, K
  REAL,PARAMETER:: PZERO = 101325.0               ! pressure at sealevel, N/sq.m
  REAL,PARAMETER:: RHOZERO = 1.2250               ! density at sealevel, kg/cu.m
  REAL,PARAMETER:: ASOUNDZERO = 340.294      ! speed of sound at sealevel, m/sec
  REAL,PARAMETER:: MUZERO = 1.7894E-5   ! viscosity at sea-level, 
  REAL,PARAMETER:: ETAZERO = 1.4607E-5 ! kinetic viscosity at sea-level, 
  REAL,PARAMETER:: KAPPA_ZERO = 0.025326            ! thermal coeff. at sea-level
                             ! units of KAPPA_ZERO are watts per meter per kelvin
  REAL,PARAMETER:: CROSS = 3.65E-10 ! collision cross-section, diatomic air m^2
  REAL,PARAMETER:: NUM_DENSITY_ZERO = RHOZERO*AVOGADRO/MOLWT_ZERO !  1/m^3
  REAL,PARAMETER:: PART_SPEED_ZERO_SQ = (8.0/PI)*RSTAR*TZERO/MOLWT_ZERO! m^2/s^2
  REAL,PARAMETER:: PART_SPEED_ZERO = SQRT((8.0/PI)*RSTAR*TZERO/MOLWT_ZERO) ! m/s

  REAL,PARAMETER:: FREE_PATH_ZERO = &
                 RSTAR*TZERO/(SQRT(2.0)*PI*AVOGADRO*CROSS*CROSS*PZERO)       ! m
  REAL,PARAMETER:: PRESSURE_SCALE_HEIGHT_ZERO = RSTAR*TZERO/(MOLWT_ZERO*GZERO)! m

! Some conversion constants for other than SI values
!!! ??  REAL,PARAMETER:: KILOWATTHOURS2KCAL = 860.0            ! exact. the definition
  REAL,PARAMETER:: BTU2KCAL = 0.2520                      ! mult BTU to get kcal
  REAL,PARAMETER:: FT2METERS = 0.3048          ! mult. ft. to get meters (exact)
  REAL,PARAMETER:: KELVIN2RANKINE = 1.8              ! mult kelvins to get deg R
  REAL,PARAMETER:: PSF2NSM = 47.880258             ! mult lb/sq.ft to get N/sq.m
  REAL,PARAMETER:: SCF2KCM = 515.379            ! mult slug/cu.ft to get kg/cu.m
  REAL,PARAMETER:: INHG2NSM = 3376.85                  ! mult inHg to get N/sq.m
  REAL,PARAMETER:: SLUG2KG = 14.594
  REAL,PARAMETER:: BTU2JOULE = 1055.0
  REAL,PARAMETER:: KCAL2JOULE = 4184.0
  REAL,PARAMETER:: INWATER2PASCAL = 248.84
  REAL,PARAMETER:: MMHG2PASCAL = 133.3224
  REAL,PARAMETER:: BARREL2LITER = 158.9873
  REAL,PARAMETER:: CALORIE2JOULE = 4.1868
  REAL,PARAMETER:: TORR2PASCAL = 133.3224   
  REAL,PARAMETER:: BAR2PASCAL = 1.0E5      

  PUBLIC::  Atmosphere
  PRIVATE:: EvaluateCubic
  PRIVATE:: KineticTemperature
  PRIVATE:: LowerAtmosphere
  PUBLIC::  MolecularWeight
  PUBLIC::  SimpleAtmosphere
  PUBLIC::  ThermalConductivity
  PUBLIC::  TransportRatios
  PRIVATE:: UpperAtmosphere
  PUBLIC::  Viscosity


CONTAINS

!+
PURE FUNCTION EvaluateCubic(a,fa,fpa, b,fb,fpb, u) RESULT(fu)
! ------------------------------------------------------------------------------
! PURPOSE - Evaluate a cubic polynomial defined by the function and the
!   1st derivative at two points.
!       Usually  a < u < b, but not necessary.  
  REAL,INTENT(IN):: u   ! point where function is to be evaluated
  REAL,INTENT(IN):: a,fa,fpa   ! a, f(a), f'(a)  at first point
  REAL,INTENT(IN):: b,fb,fpb   ! b, f(b), f'(b)  at second point


  REAL:: fu                    ! computed value of f(u)

  REAL:: d,t,p
!-------------------------------------------------------------------------------
  d=(fb-fa)/(b-a)   ! b==a will be a fatal error 
  t=(u-a)/(b-a)
  p=1.0-t

  fu = p*fa + t*fb - p*t*(b-a)*(p*(d-fpa)-t*(d-fpb))
  RETURN
END Function EvaluateCubic   ! -------------------------------------------------

!+
PURE FUNCTION KineticTemperature(z) RESULT(t)
!   ----------------------------------------------------------------------------
! PURPOSE - Compute kinetic temperature above 86 km.

  REAL,INTENT(IN)::  z     ! geometric altitude, km.                        
  REAL:: t     ! kinetic temperature, K

! TABLE 5 - DEFINITION OF KINETIC TEMPERATURE FROM 86 km to 1000 km
  REAL,PARAMETER:: C1 = -76.3232  ! uppercase A in document
  REAL,PARAMETER:: C2 = 19.9429   ! lowercase a in document
  REAL,PARAMETER:: C3 = 12.0
  REAL,PARAMETER:: C4 = 0.01875   ! lambda in document
  REAL,PARAMETER:: TC = 263.1905
  REAL,PARAMETER:: Z7 =  86.0   ! not used  
  REAL,PARAMETER:: T7 = 186.8673
  REAL,PARAMETER:: z8 =  91.0
  REAL,PARAMETER:: Z9 = 110.0,  T9=240.0
  REAL,PARAMETER:: Z10= 120.0, T10=360.0
!  REAL,PARAMETER:: Z11= 500.0, T11=999.2356   ! not used
  REAL,PARAMETER:: T12=1000.0   ! same as T sub infinity
  
! NOTE - REARTH is a module variable  


  REAL:: xx,yy
!-------------------------------------------------------------------------------
  IF (z <= Z8) THEN
    t=T7                    ! Eq (25), p.11
  ELSE IF (z < Z9) THEN  
    xx=(z-Z8)/C2                        ! from Appendix B, p.223
    yy=SQRT(1.0-xx*xx)
    t=TC+C1*yy            ! Eq (27), p.11
  ELSE IF (z <= Z10) THEN
    t=T9+C3*(z-Z9)      ! Eq (29)
  ELSE
    xx=(REARTH+Z10)/(REARTH+z)
    yy=(T12-T10)*EXP(-C4*(z-Z10)*xx)
    t=T12-yy       ! Eq. (31)
  END IF

  RETURN
END Function KineticTemperature   ! --------------------------------------------

!+  
PURE FUNCTION ThermalConductivity(t) RESULT(k)   
!   ----------------------------------------------------------------------------
! PURPOSE - Compute the coefficient of thermal conductivity at temperature t.
! This is a coding of section 1.3.13 on p. 19 of the reference document.

  REAL,INTENT(IN) :: t                ! temperature, K

  REAL:: k  ! coefficient of thermal conductivity, watts per meter per kelvin
    ! some refer to the units as joules per second per meter per kelvin
  REAL,PARAMETER:: C1=2.64638E-3, C2=245.4
! NOTE - This empirical equation is given in the reference document.
!-------------------------------------------------------------------------------
  k=C1*SQRT(t*t*t)/(t+C2*10.0**(-12.0/t))
  RETURN
END Function ThermalConductivity   ! -------------------------------------------

!+
SUBROUTINE UpperAtmosphere(alt, sigma, delta, theta)
!   ----------------------------------------------------------------------------
! PURPOSE - Compute the properties of the 1976 standard atmosphere from
!   86 km. to 1000 km.

  REAL,INTENT(IN)::  alt    ! geometric altitude, km.                        
  REAL,INTENT(OUT):: sigma  ! density/sea-level standard density              
  REAL,INTENT(OUT):: delta  ! pressure/sea-level standard pressure           
  REAL,INTENT(OUT):: theta  ! temperature/sea-level standard temperature

  INTEGER,PARAMETER:: NTABLE = 25

! altitude table (km)
  REAL,PARAMETER,DIMENSION(NTABLE):: Z = (/   &
     86.0,  93.0, 100.0, 107.0, 114.0,        &
    121.0, 128.0, 135.0, 142.0, 150.0,        &
    160.0, 170.0, 180.0, 190.0, 200.0,        &
    220.0, 260.0, 300.0, 400.0, 500.0,        &
    600.0, 700.0, 800.0, 900.0,1000.0 /)

! pressure table  (DELTA = P/P0)
  REAL,PARAMETER,DIMENSION(SIZE(Z)):: DELTA_TABLE = (/          &
    3.6850E-6, 1.0660E-6, 3.1593E-7, 1.0611E-7, 4.3892E-8,      &
    2.3095E-8, 1.3997E-8, 9.2345E-9, 6.4440E-9, 4.4828E-9,      &
    2.9997E-9, 2.0933E-9, 1.5072E-9, 1.1118E-9, 8.3628E-10,     &
    4.9494E-10, 1.9634E-10, 8.6557E-11, 1.4328E-11, 2.9840E-12, &
    8.1056E-13, 3.1491E-13, 1.6813E-13, 1.0731E-13, 7.4155E-14 /)
    

! density table  (SIGMA = RHO/RHO0)
  REAL,PARAMETER,DIMENSION(SIZE(Z)):: SIGMA_TABLE = (/     &
    5.680E-6, 1.632E-6, 4.575E-7, 1.341E-7, 4.061E-8,      &
    1.614E-8, 7.932E-9, 4.461E-9, 2.741E-9, 1.694E-9,      &
    1.007E-9, 6.380E-10, 4.240E-10, 2.923E-10, 2.074E-10,  &
    1.116E-10, 3.871E-11, 1.564E-11, 2.288E-12, 4.257E-13, &
    9.279E-14, 2.506E-14, 9.272E-15, 4.701E-15, 2.907E-15 /)
    
  REAL,PARAMETER,DIMENSION(SIZE(Z)):: LOGDELTA = LOG(DELTA_TABLE)
  REAL,PARAMETER,DIMENSION(SIZE(Z)):: LOGSIGMA = LOG(SIGMA_TABLE)

  REAL,PARAMETER,DIMENSION(SIZE(Z)):: DLOGDELTA = (/        & 
    -0.174061, -0.177924, -0.167029, -0.142755, -0.107859,  &
    -0.079322, -0.064664, -0.054879, -0.048260, -0.042767,  &
    -0.037854, -0.034270, -0.031543, -0.029384, -0.027632,  &
    -0.024980, -0.021559, -0.019557, -0.016735, -0.014530,  &
    -0.011314, -0.007677, -0.005169, -0.003944, -0.003612 /)
  REAL,PARAMETER,DIMENSION(SIZE(Z)):: DLOGSIGMA = (/   & 
    -0.172421, -0.182258, -0.178090, -0.176372, -0.154322,   &
    -0.113750, -0.090582, -0.075033, -0.064679, -0.056067,   &
    -0.048461, -0.043042, -0.038869, -0.035648, -0.033063,   &
    -0.029164, -0.024220, -0.021336, -0.017686, -0.016035,   &
    -0.014327, -0.011631, -0.008248, -0.005580, -0.004227 /)

  INTEGER:: i,j,k                                                     ! counters
!-------------------------------------------------------------------------------

  IF (alt > Z(SIZE(Z))) THEN          ! trap altitudes greater than 1000 km.
    delta=DELTA_TABLE(SIZE(Z))        ! return value for Z=1000.0
    sigma=SIGMA_TABLE(SIZE(Z))        !   ditto 
    theta=1000.0/TZERO
    RETURN
  END IF

  i=1 
  j=SIZE(Z)                                    ! setting up for binary search
  DO
    k=(i+j)/2                                              ! integer division
    IF (alt < Z(k)) THEN
      j=k
    ELSE
      i=k
    END IF   
    IF (j <= i+1) EXIT
  END DO

  delta=EXP(EvaluateCubic(Z(i), LOGDELTA(i),   DLOGDELTA(i),          &
                        Z(i+1), LOGDELTA(i+1), DLOGDELTA(i+1), alt))

  sigma=EXP(EvaluateCubic(Z(i), LOGSIGMA(i),   DLOGSIGMA(i),          &
                        Z(i+1), LOGSIGMA(i+1), DLOGSIGMA(i+1), alt))

  theta=KineticTemperature(alt)/TZERO

  RETURN
END Subroutine UpperAtmosphere   ! ---------------------------------------------

!+
SUBROUTINE LowerAtmosphere(alt, sigma, delta, theta)
!   ----------------------------------------------------------------------------
! PURPOSE - Compute the properties of the 1976 standard atmosphere to 86 km.

  REAL,INTENT(IN)::  alt    ! geometric altitude, km.                        
  REAL,INTENT(OUT):: sigma  ! density/sea-level standard density              
  REAL,INTENT(OUT):: delta  ! pressure/sea-level standard pressure           
  REAL,INTENT(OUT):: theta  ! temperature/sea-level standard temperature

  INTEGER,PARAMETER:: NTAB=8       ! number of entries in the defining tables
! NOTE - REARTH and GMR are module parameters (constants).  
! If you copy this subroutine for use elsewhere, then uncomment following:
!   REAL,PARAMETER:: REARTH = 6356.766, GMR = 34.163195

  INTEGER:: i,j,k                                                  ! counters
  REAL:: h                                       ! geopotential altitude (km)
  REAL:: tgrad, tbase      ! temperature gradient and base temp of this layer
  REAL:: tlocal                                           ! local temperature
  REAL:: deltah                             ! height above base of this layer

!===============================================================================
!      C O N S T A N T   A R R A Y S  O F  1 9 7 6   S T D.  A T M O S P H E R E
!===============================================================================
  REAL,DIMENSION(NTAB),PARAMETER:: HTAB = &
                          (/0.0, 11.0, 20.0, 32.0, 47.0, 51.0, 71.0, 84.852/)
  REAL,DIMENSION(NTAB),PARAMETER:: TTAB = &
          (/288.15, 216.65, 216.65, 228.65, 270.65, 270.65, 214.65, 186.946/)
  REAL,DIMENSION(NTAB),PARAMETER:: PTAB = &
               (/1.0, 2.233611E-1, 5.403295E-2, 8.5666784E-3, 1.0945601E-3, &
                                     6.6063531E-4, 3.9046834E-5, 3.68501E-6/)
  REAL,DIMENSION(NTAB),PARAMETER:: GTAB = &
                                (/-6.5, 0.0, 1.0, 2.8, 0.0, -2.8, -2.0, 0.0/)
!-------------------------------------------------------------------------------
  h=alt*REARTH/(alt+REARTH)      ! convert geometric to geopotential altitude

  i=1 
  j=NTAB                                          ! setting up for binary search
  DO
    k=(i+j)/2                                              ! integer division
    IF (h < htab(k)) THEN
      j=k
    ELSE
      i=k
    END IF   
    IF (j <= i+1) EXIT
  END DO

  tgrad=gtab(i)                                     ! i will be in 1...NTAB-1
  tbase=ttab(i)
  deltah=h-htab(i)
  tlocal=tbase+tgrad*deltah
  theta=tlocal/ttab(1)                                    ! temperature ratio

  IF (tgrad == 0.0) THEN                                     ! pressure ratio
    delta=ptab(i)*EXP(-GMR*deltah/tbase)
  ELSE
    delta=ptab(i)*(tbase/tlocal)**(GMR/tgrad)
  END IF

  sigma=delta/theta                                           ! density ratio
  RETURN
END Subroutine LowerAtmosphere   ! ---------------------------------------------

!+
SUBROUTINE SimpleAtmosphere(alt,sigma,delta,theta)
!   ----------------------------------------------------------------------------
! PURPOSE - Compute the characteristics of the atmosphere below 20 km.

! NOTES-Correct to 20 km. Only approximate above there

  IMPLICIT NONE

  REAL,INTENT(IN)::  alt    ! geometric altitude, km.
  REAL,INTENT(OUT):: sigma  ! density/sea-level standard density             
  REAL,INTENT(OUT):: delta  ! pressure/sea-level standard pressure            
  REAL,INTENT(OUT):: theta  ! temperature/sea-level standard temperature   

! NOTE - REARTH and GMR are module parameters (constants).  
! If you copy this subroutine for use elsewhere, then uncomment following:
!   REAL,PARAMETER:: REARTH = 6356.766, GMR = 34.163195

  REAL:: h   ! geopotential altitude
!-------------------------------------------------------------------------------
  h=alt*REARTH/(alt+REARTH)      ! convert geometric to geopotential altitude

  IF (h < 11.0) THEN
    theta=1.0+(-6.5/288.15)*h                                   ! Troposphere
    delta=theta**(GMR/6.5)
  ELSE
    theta=216.65/288.15                                        ! Stratosphere
    delta=0.2233611*EXP(-GMR*(h-11.0)/216.65)
  END IF

  sigma=delta/theta
  RETURN
END Subroutine SimpleAtmosphere   ! --------------------------------------------

!+
PURE FUNCTION Viscosity(theta) RESULT(visc)
!   ----------------------------------------------------------------------------
! PURPOSE - Compute viscosity using Sutherland's formula.
!        Returns viscosity in kg/(meter-sec)

  REAL,INTENT(IN) :: theta                ! temperature/sea-level temperature  
  REAL:: visc
  REAL:: temp                              ! temperature in kelvins
! NOTE - BETAVISC and TZERO are module parameters (constants).  
! If you copy this subroutine for use elsewhere, uncomment the following:
!  REAL,PARAMETER:: BETAVISC = 1.458E-6, SUTH = 110.4    
!-------------------------------------------------------------------------------
  temp=TZERO*theta
  visc=BETAVISC*Sqrt(temp*temp*temp)/(temp+SUTH)
  RETURN
END Function Viscosity   ! -----------------------------------------------------

!+
SUBROUTINE Atmosphere(alt,sigma,delta,theta)
!   ----------------------------------------------------------------------------
! PURPOSE - Compute the characteristics of the U.S. Standard Atmosphere 1976

  IMPLICIT NONE
  REAL,INTENT(IN)::  alt    ! geometric altitude, km.
  REAL,INTENT(OUT):: sigma  ! density/sea-level standard density             
  REAL,INTENT(OUT):: delta  ! pressure/sea-level standard pressure            
  REAL,INTENT(OUT):: theta  ! temperature/sea-level standard temperature   
!-------------------------------------------------------------------------------
  IF (alt > 86.0) THEN
    CALL UpperAtmosphere(alt,sigma,delta,theta)
  ELSE
    CALL LowerAtmosphere(alt,sigma,delta,theta)
  END IF
  RETURN
END Subroutine Atmosphere   ! --------------------------------------------------

!+
SUBROUTINE TransportRatios(alt, gRatio,dynamicViscosityRatio, &
  kinematicViscosityRatio, numberDensityRatio, collisionFrequencyRatio, &
  particleSpeedRatio, &
  meanFreePathRatio,thermalConductivityRatio, soundSpeedRatio, &
  molecularWeightRatio )
! ------------------------------------------------------------------------------
! PURPOSE - Compute various non-dimensional ratios.
  REAL,INTENT(IN):: alt   ! geometric altitude in kilometers
  REAL,INTENT(OUT):: gRatio 
  REAL,INTENT(OUT):: dynamicViscosityRatio
  REAL,INTENT(OUT):: kinematicViscosityRatio
  REAL,INTENT(OUT):: numberDensityRatio
  REAL,INTENT(OUT):: collisionFrequencyRatio
  REAL,INTENT(OUT):: particleSpeedRatio 
  REAL,INTENT(OUT):: meanFreePathRatio
  REAL,INTENT(OUT):: thermalConductivityRatio 
  REAL,INTENT(OUT):: soundSpeedRatio 
  REAL,INTENT(OUT):: molecularWeightRatio
  
  REAL:: sigma,theta,delta 
  REAL:: kappa
  REAL:: molwt
!-------------------------------------------------------------------------------
  CALL Atmosphere(alt,sigma,delta,theta)
  molwt=MolecularWeight(alt)
  molecularWeightRatio=molwt/MOLWT_ZERO
  
  gRatio=(REARTH/(alt+REARTH))**2
  dynamicViscosityRatio=Viscosity(theta)/MUZERO 
  kinematicViscosityRatio=dynamicViscosityRatio/sigma
  numberDensityRatio=sigma*MOLWT_ZERO/molwt
  particleSpeedRatio=SQRT(theta/molecularWeightRatio)
  meanFreePathRatio=molecularWeightRatio/sigma
  collisionFrequencyRatio=particleSpeedRatio/meanFreePathRatio
  kappa=ThermalConductivity(theta*TZERO)
  thermalConductivityRatio=kappa/KAPPA_ZERO  
  soundSpeedRatio=SQRT(theta)

   
  RETURN 
END Subroutine TransportRatios   ! ---------------------------------------------

!+
PURE FUNCTION MolecularWeight(altKm) RESULT(mw)
! ------------------------------------------------------------------------------
! PURPOSE - Compute molecular weight of air at altitude by interpolation
!    in tables.
  REAL,INTENT(IN):: altKm
  INTEGER,PARAMETER:: NTABLE = 25

! tables of altitude (km), mol.wt., and d(mol.wt.)/d(altKm)
  REAL,PARAMETER,DIMENSION(NTABLE):: Z = (/  &
     86.0,  93.0, 100.0, 107.0, 114.0,       &
    121.0, 128.0, 135.0, 142.0, 150.0,       &
    160.0, 170.0, 180.0, 190.0, 200.0,       &
    220.0, 260.0, 300.0, 400.0, 500.0,       &
    600.0, 700.0, 800.0, 900.0,1000.0 /)
    
  REAL,PARAMETER,DIMENSION(NTABLE):: M = (/  &
    28.95, 28.82, 28.40, 27.64, 26.79,       &
    26.12, 25.58, 25.09, 24.62, 24.10,       &
    23.49, 22.90, 22.34, 21.81, 21.30,       &
    20.37, 18.85, 17.73, 15.98, 14.33,       &
    11.51,  8.00,  5.54,  4.40,  3.94 /)  

  REAL,PARAMETER,DIMENSION(NTABLE):: MP = (/                 &
    -0.001340, -0.036993, -0.086401, -0.123115, -0.111136,   &
    -0.083767, -0.072368, -0.068190, -0.066300, -0.063131,   &
    -0.059786, -0.057724, -0.054318, -0.052004, -0.049665,   &
    -0.043499, -0.032674, -0.023804, -0.014188, -0.021444,   &
    -0.034136, -0.031911, -0.017321, -0.006804, -0.003463 /)

  REAL:: mw
  INTEGER:: i,j,k
!-------------------------------------------------------------------------------
  IF (altKm <= 86.0) THEN 
    mw = MOLWT_ZERO
  ELSE 
    i=1 
    j=SIZE(Z)                                     ! setting up for binary search
    DO
      k=(i+j)/2                                               ! integer division
      IF (altKm < Z(k)) THEN
        j=k
      ELSE
        i=k
      END IF   
      IF (j <= i+1) EXIT
    END DO
    mw = EvaluateCubic(Z(i), M(i), MP(i), Z(i+1), M(i+1), MP(i+1), altKm)
  END IF
  
  RETURN 
END Function MolecularWeight   ! -----------------------------------------------   
   
END Module Atmosphere1976   ! ==================================================

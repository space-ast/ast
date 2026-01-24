C*********************************************************************C
C*                                                                   *C
C*  testj77.for                                                      *C
C*                                                                   *C
C*  Written by:  David L. Huestis, Molecular Physics Laboratory      *C
C*                                                                   *C
C*  Copyright (c) 1999  SRI International                            *C
C*  All Rights Reserved                                              *C
C*                                                                   *C
C*  This software is provided on an as is basis; without any         *C
C*  warranty; without the implied warranty of merchantability or     *C
C*  fitness for a particular purpose.                                *C
C*                                                                   *C
C*********************************************************************C
C*
C*	Main program to test j77sri.for
C*
C*	The user should choose values of mz (maximim altitude in km)
C*	and Tinf (exospheric temperature in K).
C*
C*	Number densities are listed as log10( molecules/cubic-meter )
C*
C*  EDIT HISTORY:
C*
C*	10-10-99  DLH	Original testj77.for
C*
C*	09-xx-99  DLH	various test versions
C*
C**********************************************************************

	parameter (mz=2500)
C	parameter (Tinf= 600.0)
	parameter (Tinf=1000.0)

	dimension z(0:mz), T(0:mz), CM(0:mz), WM(0:mz)
	dimension CN2(0:mz), CO2(0:mz), CO(0:mz), CAr(0:mz),
     *	  CHe(0:mz), CH(0:mz)
	dimension v(7)
	common/COM1/Z,T,CM,WM,v
	common/COM2/CN2,CO2,CO
	common/COM3/CAr,CHe,CH

	call j77sri( mz, Tinf, z, T, 
     *	  CN2, CO2, CO, CAr, CHe, CH, CM, WM )

	do 500 i=0,mz
	  if( i .le. 80 ) then
	    if( mod(i,5) .ne. 0 ) go to 500
	  else if( i .le. 100 ) then
	    go to 400
	  else if( i .le. 110 ) then
	    if( mod(i,2) .ne. 0 ) go to 500
	  else if( i .le. 160 ) then
	    if( mod(i,5) .ne. 0 ) go to 500
	  else if( i .le. 400 ) then
	    if( mod(i,10) .ne. 0 ) go to 500
	  else if( i .le. 1000 ) then
	    if( mod(i,20) .ne. 0 ) go to 500
	  else if( i .le. 1500 ) then
	    if( mod(i,50) .ne. 0 ) go to 500
	  else
	    if( mod(i,100) .ne. 0 ) go to 500
	  end if
400	  v(1) = CN2(i)
	  v(2) = CO2(i)
	  v(3) = CO(i)
	  v(4) = CAr(i)
	  v(5) = CHe(i)
	  v(6) = CH(i)
	  v(7) = CM(i)
	  do j=1,7
	    if( v(j) .gt. 1.26E-16 ) then
	      v(j) = alog10( v(j) ) + 6.0
	    else
	      v(j) = -9.9
	    end if
	  end do
	  write(*,2000)i,t(i),v,WM(i)
500	continue
2000	format( i5,f8.2,7f8.4,f7.3)
	end


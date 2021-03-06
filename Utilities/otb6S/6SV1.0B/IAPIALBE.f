      subroutine iapialbe(pild,pxlt,prl,ptl,prs,pihs,pc,
     s           brdfalb)
c
c interface between the computer code of the model of Iaquinta and Pinty
c the computer code is courtesy of Jean Ianquinta
c see module IAPITOOLS.f for a complete description
c
c
      parameter (nta=24,nfa=48)
      real teta1,teta2,phi1,phi2,ta(nta),fa(nfa),wta(nta),wfa(nfa)
      real mu1,mu2,fi
      real pi,y
      real brdfalb,summ,si1,si2,ro_1,pond
      integer iwr,l,k,j
      integer      pild,pihs
      real         pxlt,prl,ptl,prs,pc
      logical ier
      common/sixs_ier/iwr,ier
C begin of Iaquinta and Pinty model parameter and declaration
        parameter (Pi=3.141592653589793)
        common /gauss_m/xgm (20),wgm (20),n
        real xgm,wgm
        integer n
        common /p/xLt,Rl,Tl,Rs,c,ild
        real xLt,Rl,Tl,Rs,c
        integer ild
        common /ld/a_ld,b_ld,c_ld,d_ld
        real a_ld,b_ld,c_ld,d_ld
        common /Ro/Ro_1_c,Ro_1_s,Ro_mult
        real Ro_1_c,Ro_1_s,Ro_mult
        real Theta_i,Phi_i
        real Theta_v,Phi_v
        integer ihs
c
c       xLt =  leaf area index
c       Rl = leaf reflection coefficient (Bi-Lambertian)
c       Tl = leaf transmission coefficient (Bi-Lambertian)
c       ild = leaf angle distribution :
c                                       1 = planophile
c                                       2 = erectophile
c                                       3 = plagiophile
c                                       4 = extremophile
c                                       5 = uniform
c       Rs = soil albedo
c       c = 2*r*Lambda
c
c       Ro_1_c  = single scattering by the canopy term
c       Ro_1_s  = uncollided by the leaves (or singly scattered by
c                 the soil) radiation
c                (Ro_1 = Ro_1_c + Ro_1_s)
c       Ro_mult = multiple scattering
c transfer paramater to common / / parameter struture
         ild=pild
         Xlt=pXlt
         Rl=pRl
         Tl=pTl
         Rs=pRs
         ihs=pihs
         c=pc
C
C Check parameter validity
           if ((ild.ne.1).and.
     +          (ild.ne.2).and.
     +          (ild.ne.3).and.
     +          (ild.ne.4).and.
     +          (ild.ne.5)) then
              print*,'Leaf angle distribution !'
              stop
            endif
            if (xlt.le.0.) then
              print*,'Leaf area index < 0. !'
              stop
            endif
            if (xlt.lt.1.) then
              print*,'Leaf area index < 1. !'
            endif
            if (xlt.gt.15.) then
              print*,'Leaf area index > 15. !'
            endif
            if (Rl.lt.0.) then
              print*,'Leaf reflectance < 0. !'
              stop
            endif
            if (Rl.gt..99) then
              print*,'Leaf reflectance > .99 !'
              stop
            endif
            if (Tl.lt.0.) then
              print*,'Leaf transmittance < 0. !'
              stop
            endif
            if (Tl.gt..99) then
              print*,'Leaf transmittance > .99 !'
              stop
            endif
            if (Rl+Tl.gt..99) then
              print*,'Single scattering albedo > .99 !'
              stop
            endif
            if (Rs.lt.0.) then
              print*,'Soil albedo < 0. !'
              stop
            endif
            if (Rs.gt..99) then
              print*,'Soil albedo > .99 !'
              stop
            endif
            if (c.lt.0.) then
              print*,'Hot-spot parameter < 0. !'
              stop
            endif
            if (c.gt.2.) then
              print*,'Hot-spot parameter > 2. !'
              stop
            endif
C compute leaf area angle distribution
      call lad
C
c - Hot-spot parameter
c
        if (ihs.eq.0) c=1.e-20
C
      teta1=0.
      teta2=pi/2.
      call gauss(teta1,teta2,ta,wta,nta)
      phi1=0.
      phi2=2.*pi
      call gauss(phi1,phi2,fa,wfa,nfa)
      brdfalb=0.
      summ=0.
c - Gauss's quadrature (n points)
c
        n=10
        call gauleg (-1.,1.,xgm,wgm,n)
c
      do 3 l=1,nta
      mu1=cos(ta(l))
      si1=sin(ta(l))
      Theta_i=acos(mu1)
      Theta_i=Pi-Theta_i
c - Computation of the multiple scattering (Ro_mult)
c
        call solve (Theta_i)
c
      do 1 k=1,nfa
      do 2 j=1,nta
      mu2=cos(ta(j))
      si2=sin(ta(j))
      fi=fa(k)
      Theta_v=acos(mu2)
      if (fi.lt.0.) fi=fi+2.*pi
      if (fi.gt.(2.*pi)) fi=fi-2.*pi
      Phi_i=fi
      Phi_v=0.
      y=Ro_1(Theta_i,Phi_i,Theta_v,Phi_v)+Ro_mult
      pond=mu1*mu2*si1*si2*wfa(k)*wta(j)*wta(l)
      brdfalb=brdfalb+pond*y
      summ=summ+pond
  2   continue
  1   continue
  3   continue
      brdfalb=brdfalb/summ
      return
      end

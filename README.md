# RFPE
This program is find to RFPE(Resilient Finite Plan Equilibria) in Repeated Games with Private Monitoring 

### Dependencies
* linbox 1.3.0（1.3.2ではなく，1.3.0です．）
* GMP 5.1.1
* gtk-cairo 1.12.2
* cddlib 0.94g
* boost_regex 1.49.0
* cblas

### TODO
* extend to multiple (over 2) players

### Update History
* **2013/01/09**
  - Calculate normal condition
  - Make Profile class for ActionProfile, StateProfile and SignalProfile
* **2013/01/03**
  - Let handle belief division as union of polytope
  - Add class Parser
  - Able to apply asymmetric automaton
* **2012/12/19**
  - Change double type to GMP rational type
  - Delete class Point(integrate into class Belief)
* **2012/12/14**
  - Make backup before modifying to use GMP
* **2012/12/13**
  - RepeatedGame.cpp, Util.cpp - modify to support linear equation in polytope condition
* **2012/12/12**
  - Add checking seminal routine
* **2012/12/05**
  - Add IO routine
* **2012/11/28**
  - Add visualization for 2, 3 states
* **2012/11/27**
  - Add calculation routine of closed belief division using cdd
* **2012/11/23**
  - First version

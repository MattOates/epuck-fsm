# This file was automatically generated by SWIG (http://www.swig.org).
# Version 1.3.36
#
# Don't modify this file, modify the SWIG interface instead.

package Robot;
use base qw(Exporter);
use base qw(DynaLoader);
package Robotc;
bootstrap Robot;
package Robot;
@EXPORT = qw();

# ---------- BASE METHODS -------------

package Robot;

sub TIEHASH {
    my ($classname,$obj) = @_;
    return bless $obj, $classname;
}

sub CLEAR { }

sub FIRSTKEY { }

sub NEXTKEY { }

sub FETCH {
    my ($self,$field) = @_;
    my $member_func = "swig_${field}_get";
    $self->$member_func();
}

sub STORE {
    my ($self,$field,$newval) = @_;
    my $member_func = "swig_${field}_set";
    $self->$member_func($newval);
}

sub this {
    my $ptr = shift;
    return tied(%$ptr);
}


# ------- FUNCTION WRAPPERS --------

package Robot;

*initRobot = *Robotc::initRobot;
*robotToString = *Robotc::robotToString;
*robotDebug = *Robotc::robotDebug;
*updateRobotState = *Robotc::updateRobotState;
*getRobotIR = *Robotc::getRobotIR;
*getRobotAcc = *Robotc::getRobotAcc;
*getRobotSteps = *Robotc::getRobotSteps;
*waitWhileCrowded = *Robotc::waitWhileCrowded;
*waitForRandDecay = *Robotc::waitForRandDecay;
*closeRobot = *Robotc::closeRobot;

# ------- VARIABLE STUBS --------

package Robot;

*spi = *Robotc::spi;
1;

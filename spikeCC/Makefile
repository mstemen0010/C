#
# File:     %M%	SCCS: %I% - %G%
# Author:   Matt Stemen
#
# Created:  12/05/1992
# Edited:   
#
# The use, disclosure, reproduction, modification, transfer, or
# transmittal of this work for any purpose in any form or by any
# means without the written permission of United Parcel Services
# is strictly prohibited.
#
# Confidential, Unpublished Property of United Parcel Services.
# Use and Distribution Limited Solely to Authorized Personnel.
#
# Copyright (c) 1992, United Parcel Service of America, Inc.
# All Rights Reserved.
#
# Purpose:
#    Build spike.
#
.KEEP_STATE :

.INIT :
	@echo "Copyright (c) 1995 by Muppet Labs Inc."
	@echo "All rights reserved."
	@echo ""

SCCSGETFLAGS=	-s

 CC = g++ -I /usr/local/lib/g++-include -lg++
 LD=     $(CC)
 LDLIBS= -L. 

EXE=		spike

OBJECTS=	table.o daemon.o spike.o spikeapp.o


$(EXE) : $(OBJECTS)
	$(LD) $(LDFLAGS) -o a.out $(OBJECTS)  $(LDLIBS)
	mv a.out $(EXE)



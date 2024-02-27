GenDir     = gen
BricksIn   = ..\..\bricks\bricks.i 
BricksHdr  = ..\..\bricks\bricks.h
WrapperGen = $(GenDir)\bricks_java_wrap.c 
Classes    = $(GenDir)\bricks.class $(GenDir)\bricksConstants.class $(GenDir)\bricksJNI.class $(GenDir)\bricks_error_code_e.class $(GenDir)\SWIGTYPE_p_int.class
Java       = $(GenDir)\bricks.java  $(GenDir)\bricksConstants.java  $(GenDir)\bricksJNI.java  $(GenDir)\bricks_error_code_e.java  $(GenDir)\SWIGTYPE_p_int.java

BricksJar  =   $(GenDir)\bricks.jar

.SUFFIXES  : .java .class

all:  $(Classes) $(BricksJar)

$(BricksJar): $(Java) $(Classes)
	jar cvf $(BricksJar) $(Classes)

{$(GenDir)\}.java{$(GenDir)\}.class::
	javac -d $(GenDir) $<
	
$(Java):: $(GenDir) $(BricksIn) $(BricksHdr)
	swig -java -o $(WrapperGen) -outdir $(GenDir) $(BricksIn) 

$(GenDir): 
	mkdir $(GenDir)

$(BricksIn):

$(BricksHdr):

clean: 
    del /Q $(GenDir)\*.*
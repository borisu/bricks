GenDir     = gen
BricksIn   = ..\..\bricks\bricks.i
WrapperGen = $(GenDir)\bricks_java_wrap.c 
JavaGen    = $(GenDir)\bricksJNI.java $(GenDir)\bricks.java $(GenDir)\bricksConstants.java
AllGen     = $(JavaGen) $(WrapperGen)
Classes    = $(GenDir)\bricksJNI.class $(GenDir)\bricks.class
OutDir     = .	
BricksJar  = $(GenDir)\bricks.jar     

all: gen $(BricksJar)  

gen: 
	if not exist $(GenDir) mkdir $(GenDir)

$(BricksJar): $(Classes)
	jar cvf $(BricksJar) $(Classes)

$(Classes): $(JavaGen)
	javac -d $(GenDir) $(JavaGen)
	
$(JavaGen): $(BricksIn)
	swig -java -o $(WrapperGen) -outdir $(GenDir) $(BricksIn) 

$(BricksIn):

clean: 
    del /Q $(BricksJar) $(AllGen) $(Classes)
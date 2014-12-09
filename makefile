CC		= g++
DEBUG		= -g
OPTIMIZE	= -O
CFLAGS		= $(OPTIMIZE) -fpermissive -w -I/gpfs/fs0/data/reddylab/bmajoros/gsl/include
LDFLAGS		= $(OPTIMIZE)
BOOM		= BOOM
OBJ		= obj
LIBS		= -LBOOM -lBOOM -LGSL -lgsl -lm -lgslcblas
#---------------------------------------------------------
$(OBJ)/gff-intersect.o:\
		gff-intersect.C
	$(CC) $(CFLAGS) -o $(OBJ)/gff-intersect.o -c \
		gff-intersect.C
#---------------------------------------------------------
gff-intersect: \
		$(OBJ)/gff-intersect.o
	$(CC) $(LDFLAGS) -o gff-intersect \
		$(OBJ)/gff-intersect.o \
		$(LIBS)
#---------------------------------------------------------
$(OBJ)/connected-components.o:\
		connected-components.C
	$(CC) $(CFLAGS) -o $(OBJ)/connected-components.o -c \
		connected-components.C
#---------------------------------------------------------
connected-components: \
		$(OBJ)/connected-components.o
	$(CC) $(LDFLAGS) -o connected-components \
		$(OBJ)/connected-components.o \
		$(LIBS)
#---------------------------------------------------------
$(OBJ)/k-medoids.o:\
		k-medoids.C
	$(CC) $(CFLAGS) -o $(OBJ)/k-medoids.o -c \
		k-medoids.C
#---------------------------------------------------------
k-medoids: \
		$(OBJ)/k-medoids.o
	$(CC) $(LDFLAGS) -o k-medoids \
		$(OBJ)/k-medoids.o \
		$(LIBS)
#---------------------------------------------------------
$(OBJ)/compile-wig.o:\
		compile-wig.C
	$(CC) $(CFLAGS) -o $(OBJ)/compile-wig.o -c \
		compile-wig.C
#---------------------------------------------------------
compile-wig: \
		$(OBJ)/compile-wig.o
	$(CC) $(LDFLAGS) -o compile-wig \
		$(OBJ)/compile-wig.o \
		$(LIBS)
#---------------------------------------------------------
$(OBJ)/txt-to-binwig.o:\
		txt-to-binwig.C
	$(CC) $(CFLAGS) -o $(OBJ)/txt-to-binwig.o -c \
		txt-to-binwig.C
#---------------------------------------------------------
txt-to-binwig: \
		$(OBJ)/txt-to-binwig.o
	$(CC) $(LDFLAGS) -o txt-to-binwig \
		$(OBJ)/txt-to-binwig.o \
		$(LIBS)
#---------------------------------------------------------
$(OBJ)/dump-binwig.o:\
		dump-binwig.C
	$(CC) $(CFLAGS) -o $(OBJ)/dump-binwig.o -c \
		dump-binwig.C
#---------------------------------------------------------
dump-binwig: \
		$(OBJ)/dump-binwig.o
	$(CC) $(LDFLAGS) -o dump-binwig \
		$(OBJ)/dump-binwig.o \
		$(LIBS)
#---------------------------------------------------------
$(OBJ)/foldchange.o:\
		foldchange.C
	$(CC) $(CFLAGS) -o $(OBJ)/foldchange.o -c \
		foldchange.C
#---------------------------------------------------------
foldchange: \
		$(OBJ)/foldchange.o
	$(CC) $(LDFLAGS) -o foldchange \
		$(OBJ)/foldchange.o \
		$(LIBS)
#---------------------------------------------------------

#--------------------------------------------------------
$(OBJ)/beta-binomial.o:\
		beta-binomial.C
	$(CC) $(CFLAGS) -o $(OBJ)/beta-binomial.o -c \
		beta-binomial.C
#---------------------------------------------------------
beta-binomial: \
		$(OBJ)/beta-binomial.o
	$(CC) $(LDFLAGS) -o beta-binomial \
		$(OBJ)/beta-binomial.o \
		$(LIBS)
#---------------------------------------------
#--------------------------------------------------------
$(OBJ)/scramble.o:\
		scramble.C
	$(CC) $(CFLAGS) -o $(OBJ)/scramble.o -c \
		scramble.C
#---------------------------------------------------------
scramble: \
		$(OBJ)/scramble.o
	$(CC) $(LDFLAGS) -o scramble \
		$(OBJ)/scramble.o \
		$(LIBS)
#--------------------------------------------------------
$(OBJ)/KDE.o:\
		KDE.C\
		KDE.H
	$(CC) $(CFLAGS) -o $(OBJ)/KDE.o -c \
		KDE.C
#--------------------------------------------------------
$(OBJ)/train-kde.o:\
		train-kde.C
	$(CC) $(CFLAGS) -o $(OBJ)/train-kde.o -c \
		train-kde.C
#---------------------------------------------------------
train-kde: \
		$(OBJ)/KDE.o \
		$(OBJ)/train-kde.o
	$(CC) $(LDFLAGS) -o train-kde \
		$(OBJ)/KDE.o \
		$(OBJ)/train-kde.o \
		$(LIBS)
#--------------------------------------------------------
$(OBJ)/kde-tail.o:\
		kde-tail.C
	$(CC) $(CFLAGS) -o $(OBJ)/kde-tail.o -c \
		kde-tail.C
#---------------------------------------------------------
kde-tail: \
		$(OBJ)/KDE.o \
		$(OBJ)/kde-tail.o
	$(CC) $(LDFLAGS) -o kde-tail \
		$(OBJ)/KDE.o \
		$(OBJ)/kde-tail.o \
		$(LIBS)
#--------------------------------------------------------
$(OBJ)/train-histogram.o:\
		train-histogram.C
	$(CC) $(CFLAGS) -o $(OBJ)/train-histogram.o -c \
		train-histogram.C
#---------------------------------------------------------
train-histogram: \
		$(OBJ)/train-histogram.o
	$(CC) $(LDFLAGS) -o train-histogram \
		$(OBJ)/train-histogram.o \
		$(LIBS)
#--------------------------------------------------------
$(OBJ)/histogram-tail.o:\
		histogram-tail.C
	$(CC) $(CFLAGS) -o $(OBJ)/histogram-tail.o -c \
		histogram-tail.C
#---------------------------------------------------------
histogram-tail: \
		$(OBJ)/histogram-tail.o
	$(CC) $(LDFLAGS) -o histogram-tail \
		$(OBJ)/histogram-tail.o \
		$(LIBS)
#--------------------------------------------------------
$(OBJ)/sample-gaussian.o:\
		sample-gaussian.C
	$(CC) $(CFLAGS) -o $(OBJ)/sample-gaussian.o -c \
		sample-gaussian.C
#---------------------------------------------------------
sample-gaussian: \
		$(OBJ)/sample-gaussian.o
	$(CC) $(LDFLAGS) -o sample-gaussian \
		$(OBJ)/sample-gaussian.o \
		$(LIBS)
#--------------------------------------------------------
$(OBJ)/plot-kde.o:\
		plot-kde.C
	$(CC) $(CFLAGS) -o $(OBJ)/plot-kde.o -c \
		plot-kde.C
#---------------------------------------------------------
plot-kde: \
		$(OBJ)/plot-kde.o \
		$(OBJ)/KDE.o
	$(CC) $(LDFLAGS) -o plot-kde \
		$(OBJ)/plot-kde.o \
		$(OBJ)/KDE.o \
		$(LIBS)
#---------------------------------------------------------


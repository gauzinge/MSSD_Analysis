INCLUDEFLAGS=-Iinclude/
COMPILERFLAGS+=-Wall
COMPILERFLAGS+=-Werror
COMPILERFLAGS+=-O0
COMPILERFLAGS+=-ggdb
ROOTFLAGS=`root-config --cflags`
ROOTLIBDIR=`root-config --libdir`
ROOTLIBFLAGS=`root-config --libs`


COMP=g++ $(COMPILERFLAGS) $(INCLUDEFLAGS)

all: plot process compare binary test

lib/style.o: src/style.cc include/style.h
	$(COMP) $(ROOTFLAGS) -c src/style.cc -o lib/style.o

lib/langaus.o: src/langaus.cc include/langaus.h
	$(COMP) $(ROOTFLAGS) -c src/langaus.cc -o lib/langaus.o

lib/Detector.o: src/Detector.cc include/Detector.h
	$(COMP) $(ROOTFLAGS) -c src/Detector.cc -o lib/Detector.o

lib/Alignment.o: src/Alignment.cc include/Alignment.h
	$(COMP) $(ROOTFLAGS) -c src/Alignment.cc -o lib/Alignment.o

lib/Noise_estimate.o: src/Noise_estimate.cc include/Noise_estimate.h
	$(COMP) $(ROOTFLAGS) -c src/Noise_estimate.cc -o lib/Noise_estimate.o

lib/Cluster.o: src/Cluster.cc include/Cluster.h
	$(COMP) $(ROOTFLAGS) -c src/Cluster.cc -o lib/Cluster.o

lib/ConfigReader.o: src/ConfigReader.cc include/ConfigReader.h
	$(COMP) $(ROOTFLAGS) -c src/ConfigReader.cc -o lib/ConfigReader.o

lib/Data.o: src/Data.cc include/Data.h
	$(COMP) $(ROOTFLAGS) -c src/Data.cc -o lib/Data.o

lib/Functions.o: src/Functions.cc include/Functions.h
	$(COMP) $(ROOTFLAGS) -c src/Functions.cc -o lib/Functions.o

lib/hitMatch.o: src/hitMatch.cc include/hitMatch.h
	$(COMP) $(ROOTFLAGS) -c src/hitMatch.cc -o lib/hitMatch.o

lib/Track.o: src/Track.cc include/Track.h
	$(COMP) $(ROOTFLAGS) -c src/Track.cc -o lib/Track.o

lib/trackfunctions.o: src/trackfunctions.cc include/trackfunctions.h
	$(COMP) $(ROOTFLAGS) -c src/trackfunctions.cc -o lib/trackfunctions.o

lib/hitCollection.o: src/hitCollection.cc include/hitCollection.h
	$(COMP) $(ROOTFLAGS) -c src/hitCollection.cc -o lib/hitCollection.o

lib/Trackhit.o: src/Trackhit.cc include/Trackhit.h
	$(COMP) $(ROOTFLAGS) -c src/Trackhit.cc -o lib/Trackhit.o

lib/CollectionFunctions.o: src/CollectionFunctions.cc include/CollectionFunctions.h
	$(COMP) $(ROOTFLAGS) -c src/CollectionFunctions.cc -o lib/CollectionFunctions.o

lib/onTrackCluster.o: src/onTrackCluster.cc include/onTrackCluster.h
	$(COMP) $(ROOTFLAGS) -c src/onTrackCluster.cc -o lib/onTrackCluster.o

lib/Settings.o: src/Settings.cc include/Settings.h
	$(COMP) $(ROOTFLAGS) -c src/Settings.cc -o lib/Settings.o

lib/DetCurve.o: src/DetCurve.cc include/DetCurve.h
	$(COMP) $(ROOTFLAGS) -c src/DetCurve.cc -o lib/DetCurve.o

lib/ResultPlotter.o: src/ResultPlotter.cc include/ResultPlotter.h
	$(COMP) $(ROOTFLAGS) -c src/ResultPlotter.cc -o lib/ResultPlotter.o

lib/FluencePlotter.o: src/FluencePlotter.cc include/FluencePlotter.h
	$(COMP) $(ROOTFLAGS) -c src/FluencePlotter.cc -o lib/FluencePlotter.o

lib/fixedVoltage.o: src/fixedVoltage.cc include/fixedVoltage.h
	$(COMP) $(ROOTFLAGS) -c src/fixedVoltage.cc -o lib/fixedVoltage.o
	
lib/BinaryCluster.o: src/BinaryCluster.cc include/BinaryCluster.h
	$(COMP) $(ROOTFLAGS) -c src/BinaryCluster.cc -o lib/BinaryCluster.o

lib/chi2dist.o: src/chi2dist.cc include/chi2dist.h
	$(COMP) $(ROOTFLAGS) -c src/chi2dist.cc -o lib/chi2dist.o

lib/pedenoise.o: src/pedenoise.cc include/pedenoise.h
	$(COMP) $(ROOTFLAGS) -c src/pedenoise.cc -o lib/pedenoise.o

lib/efficiency.o: src/efficiency.cc include/efficiency.h
	$(COMP) $(ROOTFLAGS) -c src/efficiency.cc -o lib/efficiency.o
	
lib/resolution_scatterplot.o: src/resolution_scatterplot.cc include/resolution_scatterplot.h
	$(COMP) $(ROOTFLAGS) -c src/resolution_scatterplot.cc -o lib/resolution_scatterplot.o
	
plot: src/plot.cc \
	lib/CollectionFunctions.o \
	lib/Trackhit.o \
	lib/trackfunctions.o \
	lib/Track.o \
	lib/hitMatch.o \
	lib/Functions.o \
	lib/Data.o \
	lib/ConfigReader.o \
	lib/Cluster.o \
	lib/Detector.o \
	lib/Alignment.o \
	lib/Noise_estimate.o \
	lib/onTrackCluster.o \
	lib/hitCollection.o \
	lib/langaus.o \
	lib/style.o \
	lib/Settings.o \
	lib/efficiency.o
	$(COMP) $(ROOTFLAGS) $(ROOTLIBFLAGS) \
	src/plot.cc \
	lib/CollectionFunctions.o \
	lib/Trackhit.o \
	lib/trackfunctions.o \
	lib/Track.o \
	lib/hitMatch.o \
	lib/Functions.o \
	lib/Data.o \
	lib/ConfigReader.o \
	lib/Cluster.o \
	lib/Detector.o \
	lib/Alignment.o \
	lib/Noise_estimate.o \
	lib/onTrackCluster.o \
	lib/hitCollection.o \
	lib/langaus.o \
	lib/style.o \
	lib/efficiency.o \
	lib/Settings.o -o plot


process: src/process.cc \
	lib/Trackhit.o \
	lib/trackfunctions.o \
	lib/Track.o \
	lib/hitMatch.o \
	lib/Functions.o \
	lib/Data.o \
	lib/ConfigReader.o \
	lib/Cluster.o \
	lib/Alignment.o \
	lib/langaus.o \
	lib/onTrackCluster.o \
	lib/Detector.o \
	lib/hitCollection.o \
	lib/pedenoise.o \
	lib/Settings.o
	$(COMP) $(ROOTFLAGS) $(ROOTLIBFLAGS) \
	src/process.cc \
	lib/Trackhit.o \
	lib/trackfunctions.o \
	lib/Track.o \
	lib/hitMatch.o \
	lib/Functions.o \
	lib/Data.o \
	lib/ConfigReader.o \
	lib/Cluster.o \
	lib/Alignment.o \
	lib/langaus.o \
	lib/onTrackCluster.o \
	lib/hitCollection.o \
	lib/Detector.o \
	lib/pedenoise.o \
	lib/Settings.o -o process


compare: src/compare.cc \
	lib/Detector.o \
	lib/Settings.o \
	lib/DetCurve.o \
	lib/FluencePlotter.o \
	lib/trackfunctions.o \
	lib/fixedVoltage.o \
	lib/chi2dist.o \
	lib/ResultPlotter.o \
	lib/style.o \
	lib/resolution_scatterplot.o
	$(COMP) $(ROOTFLAGS) $(ROOTLIBFLAGS) \
	src/compare.cc \
	lib/Detector.o \
	lib/Settings.o \
	lib/DetCurve.o \
	lib/fixedVoltage.o \
	lib/trackfunctions.o \
	lib/chi2dist.o \
	lib/FluencePlotter.o \
	lib/ResultPlotter.o \
	lib/resolution_scatterplot.o \
	lib/style.o -o compare


binary: src/binary.cc \
	lib/Trackhit.o \
	lib/trackfunctions.o \
	lib/Track.o \
	lib/hitMatch.o \
	lib/Data.o \
	lib/ConfigReader.o \
	lib/onTrackCluster.o \
	lib/Cluster.o \
	lib/Alignment.o \
	lib/BinaryCluster.o \
	lib/langaus.o \
	lib/hitCollection.o \
	lib/Settings.o \
	lib/Functions.o \
	lib/Detector.o \
	lib/style.o
	$(COMP) $(ROOTFLAGS) $(ROOTLIBFLAGS) \
	src/binary.cc \
	lib/Trackhit.o \
	lib/trackfunctions.o \
	lib/Track.o \
	lib/hitMatch.o \
	lib/Data.o \
	lib/Settings.o \
	lib/ConfigReader.o \
	lib/hitCollection.o \
	lib/Cluster.o \
	lib/Alignment.o \
	lib/BinaryCluster.o \
	lib/Functions.o \
	lib/langaus.o \
	lib/onTrackCluster.o \
	lib/Detector.o \
	lib/style.o -o binary

test: src/test.cc \
	lib/ConfigReader.o \
	lib/Data.o \
	lib/hitMatch.o \
	lib/pedenoise.o \
	lib/Cluster.o \
	lib/Track.o \
	lib/Alignment.o \
	lib/onTrackCluster.o \
	lib/hitCollection.o \
	lib/Trackhit.o \
	lib/langaus.o \
	lib/Detector.o \
	lib/Functions.o \
	lib/trackfunctions.o \
	lib/efficiency.o \
	lib/Settings.o 
	$(COMP) $(ROOTFLAGS) $(ROOTLIBFLAGS) \
	src/test.cc \
	lib/ConfigReader.o \
	lib/Data.o \
	lib/Detector.o \
	lib/hitMatch.o \
	lib/langaus.o \
	lib/Cluster.o \
	lib/hitCollection.o \
	lib/onTrackCluster.o \
	lib/Trackhit.o \
	lib/Functions.o \
	lib/Track.o \
	lib/Alignment.o \
	lib/trackfunctions.o \
	lib/efficiency.o \
	lib/Settings.o \
	lib/pedenoise.o -o test

clean:
	rm -f process plot lib/*.o

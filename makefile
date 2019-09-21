CXX       = pgc++
GPUFLAG  = -ta=tesla:cc35 -acc -Minfo=accel
FLAGS    = -fast -O3
CXXFLAGS = $(FLAGS) $(GPUFLAG)
LDFLAGS  = $(GPUFLAG) #-lm

mandelbox: main.o print.o timing.o savebmp.o getparams.o 3d.o getcolor.o \
	raymarching_box.o renderer.o init3D.o savedata.o hitdata.o smoothmove.o
	$(CXX) -o $@ $? $(LDFLAGS)

mandelbulb: main.o print.o timing.o savebmp.o getparams.o 3d.o getcolor.o \
	raymarching_bulb.o renderer.o init3D.o
	$(CXX) -o $@ $? $(LDFLAGS)

clean:
	rm -f *.o mandelbox mandelbulb *~ gmon.out profout

rebox:
	make clean && make

rebulb:
	make clean && make mandelbulb

prof:
	gprof $(PROGRAM_NAME) gmon.out > profout && \
	./gprof2dot.py profout | dot -Tpng -o profout.png

runbox:
	./mandelbox paramsBox.dat

runbulb:
	./mandelbulb paramsBulb.dat

runboxjpg:
	./mandelbox paramsBox.dat && ./convertJPG

runvideo:
	./mandelbox paramsBox.dat && ./convertJPG && ./genvideo &&rm *.jpg

diff:
	diff image.bmp image_GPU.bmp

diffbulb:
	diff imageBulb.bmp imageBulb_GPU.bmp

watch:
	watch -n 0.1 nvidia-smi

copybulb:
	scp guz9@mills.mcmaster.ca:/u50/guz9/6F03_Project/imageBulb.bmp /home/godric/Program/6F03_Project/

copybox:
	scp guz9@moore.mcmaster.ca:/u50/guz9/6F03_Project/image*.bmp /home/godric/Program/6F03_Project/

copyvideo:
	scp guz9@moore.mcmaster.ca:/u50/guz9/6F03_Project/*.mpg /home/godric/Program/6F03_Project/

cleanbox:
	rm image*

copypar:
	scp  /home/godric/Program/6F03_Project/paramsBox.dat guz9@moore.mcmaster.ca:/u50/guz9/6F03_Project/

copy:
	scp -r /home/godric/Program/6F03_Project/ guz9@moore.mcmaster.ca:

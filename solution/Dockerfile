FROM ubuntu:jammy
RUN apt update
RUN apt -y install gcc build-essential
WORKDIR /home/pet
COPY Makefile .
COPY tracer.c .
COPY main.cc .
RUN make
RUN rm tracer.c Makefile main.cc
CMD ["/home/pet/tracer"]

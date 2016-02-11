
options = --board attiny:avr:attiny:cpu=attiny84,clock=internal1

all: verify

# aliases
install: upload
build: verify

upload:
	arduino $(options) --upload .

verify:
	arduino $(options) --verify .

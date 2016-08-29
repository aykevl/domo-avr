
options = --board breadboard:avr:atmega328bb --useprogrammer

all: verify

# aliases
install: upload
build: verify

upload:
	arduino --upload $(options) .

verify:
	arduino --verify $(options) .

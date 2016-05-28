# invoke SourceDir generated makefile for i2ctmp006.pem4f
i2ctmp006.pem4f: .libraries,i2ctmp006.pem4f
.libraries,i2ctmp006.pem4f: package/cfg/i2ctmp006_pem4f.xdl
	$(MAKE) -f C:\Users\u400\I2C_Draft/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\u400\I2C_Draft/src/makefile.libs clean


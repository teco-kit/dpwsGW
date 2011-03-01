.PHONY: clean ccsc_clean install

CFLAGS+=I+=$(PBS) +FH +STDOUT +EA +T -J +Y=0
CC=/cygdrive/c/Program\ Files/PICC/Ccsc
#CFLAGS+=I+=$(PBS) +FH +T
ICD="`cygpath "$$PROGRAMFILES"`/PICC/ccsload.exe" WRITE=

clean: ccsc_clean 

ccsc_clean: 
	@rm -f *.hex *.HEX *.ERR *.err *.COD *.cod *.d *.sym *.SYM *.esym *.tre
	@echo cleaned.

install:
	$(ICD)$<
#	icd -T$<
	
	

%.hex:%.c
	$(CC) $(CFLAGS) 2>&1 $(@:.hex=.c) $(FMT_OUT)
	-@[ -f $(@:.hex=.cod) ] && $(PBS)/Mk/mkdepend.sh $(@:.hex=.cod) >$(@:.hex=.d) 2>/dev/null


-include *.d

FMT_OUT?=|awk -F'"' '//{for(i=2;i<=NF;i+=2)gsub(" ","::",$$i);print $$0}' |awk 'BEGIN{exitcode=0;} /Errors/||/RAM=/{print;next;} \
		//{\
		gsub("\"","",$$4);\
		gsub("::"," ",$$4);\
		file=$$4;\
		err=$$2" "$$3;\
		line=$$6;\
		text=$$0;\
		if($$2=="Error")exitcode=$$3;printf("%s:%d:%s: %s\n",file,line,err,text);}\
		END{exit exitcode;}'

FORCE:

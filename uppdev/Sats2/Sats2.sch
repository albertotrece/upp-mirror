TABLE_(Area)
	INT_	(ID) PRIMARY_KEY
	STRING_	(Name, 500)
END_TABLE

TABLE_(SM)
	INT		(ID) PRIMARY_KEY
	STRING_	(FName, 500)
	STRING_	(LName, 500)
	STRING_ (MI, 500)
	INT_	(GenID)
	INT_	(SSN)
	DATE_	(EnlDate)
	INT_	(AFQT)
	INT_	(EdGrade)
	STRING_	(EdCodeID, 500)
	STRING_	(PRN, 500)
	STRING_	(MOS, 500)
	INT_	(Zip)
	DATE_	(ShipDate)
	INT_	(EnlTypeID)
	STRING_	(SexID, 500)
	INT_	(RaceID)
	INT_	(EthnicID)
	INT_	(RRNCOID)
	STRING_	(Remarks, 500)
	STRING_	(RemarksII, 500)
END_TABLE

TABLE_(Sex)
	STRING_	(Code, 500)
END_TABLE

TABLE_(Race)
	INT		(ID) PRIMARY_KEY
	STRING	(Code, 500)
END_TABLE

TABLE_(Ethnic)
	INT		(ID) PRIMARY_KEY
	STRING	(Code, 500)
END_TABLE

TABLE_(EdCode)
	STRING	(ID, 500) PRIMARY_KEY
	STRING	(Code, 500)
END_TABLE

TABLE_(Generation)
	INT		(ID) PRIMARY_KEY
	STRING	(Code, 500)
END_TABLE

TABLE_(RRNCO)
	INT_	(DBID) PRIMARY_KEY
	STRING_	(RSID, 500)
	STRING	(FName, 500)
	STRING	(LName, 500)
	INT_	(AreaID)
END_TABLE

TABLE_(AreaMission)
	INT		(ID) PRIMARY_KEY
	INT_	(PS)
	INT_	(NPS)
	INT_	(IST)
	INT_	(ISR)
END_TABLE

TABLE_(EnlType)
	INT		(ID) PRIMARY_KEY
	STRING	(Code, 500)
END_TABLE

TABLE_(OpsData)
	INT		(ID) PRIMARY_KEY
	STRING_	(QC72, 500)
	STRING_	(TOS, 500)
	STRING_	(WTC, 500)
	STRING_	(Physical, 500)
	STRING_	(Load, 500)
	STRING	(Remarks, 500)
	STRING_	(ManualScan, 500)
	STRING_	(SIDPERS, 500)
END_TABLE

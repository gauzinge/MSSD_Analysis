#!/usr/bin/awk -f

function prepareColumns() {
  column[0]=-1;
  column[1]=-1;
  column[2]=-1;
  column[3]=-1;
    
  dut[1]=-1;
  dut[2]=-1;
  dut[3]=-1;
  dut[4]=-1;
}

BEGIN {
  linecount=0;
  RunNumber=-1;
  columnIndex[0]=0;
  columnIndex[1]=1;
  columnIndex[2]=2;
  columnIndex[3]=3;
  prepareColumns();
}

function printLine() {
  if (RunNumber!=-1) {
    nextLine=RunNumber;
    for (i=0; i<4; ++i) {
      cindex=columnIndex[i];
      nextLine=nextLine" "column[cindex];
    }
    print nextLine;
  }
  prepareColumns();
}


/^Run/ {
  printLine();
  RunNumber=$2*1;
}

/^CAEN/ {
  colIndex=0;
  column[colIndex]=$3*1;
  colIndex=1;
  column[colIndex]=$5*1;
  colIndex=2;
  column[colIndex]=$7*1;
  colIndex=3;
  column[colIndex]=$9*1;
}

END {
  printLine();
}

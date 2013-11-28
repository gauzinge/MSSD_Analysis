#!/usr/bin/awk -f

function prepareColumns() {
  column[20]=-1;
  column[21]=-1;
  column[22]=-1;
  column[23]=-1;
  column[24]=-1;
  column[25]=-1;
  column[26]=-1;
  column[27]=-1;
  
  dut[1]=-1;
  dut[2]=-1;
  dut[3]=-1;
  dut[4]=-1;
}

BEGIN {
  linecount=0;
  RunNumber=-1;
  columnIndex[0]=20;
  columnIndex[1]=21;
  columnIndex[2]=22;
  columnIndex[3]=23;
  columnIndex[4]=24;
  columnIndex[5]=25;
  columnIndex[6]=26;
  columnIndex[7]=27;
  prepareColumns();
}

function printLine() {
  if (RunNumber!=-1) {
    nextLine=RunNumber;
    for (i=0; i<8; ++i) {
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

/^ch/ {
  colIndex=$2*1;
  column[colIndex]=$4*1;
}

END {
  printLine();
}

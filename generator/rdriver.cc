#include "rdriver.h"
#include "io.h"
#include "drivers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RDriverParser::RDriverParser(const RNodeReader &_rnr, const std::vector<uint8_t> &data) : rnr(_rnr), drivermatch(DriversParser::driver_type_names)
{
  p = data.data();
  e = data.data() + data.size();
  next();
}

void RDriverParser::error(const uint8_t *st, const char *err) const
{
  if(err)
    fprintf(stderr, "%s\n", err);
  const uint8_t *en = st;
  while(*en != '\n' && *en != '\r')
    en++;
  std::string line(st, en);
  fprintf(stderr, "line: %s\n", line.c_str());
  exit(1);
};

void RDriverParser::next()
{
  drivers[0] = drivers[1] = 0xff;

  if(p == e) {
    rn = 0;
    return;
  }

  const uint8_t *st = p;

 rerere:
  rn = rnr.lookup(p);
  if(!rn)
    error(st);
  if(*p++ != ' ')
    error(st, "Space expected after source node");

  int dr1 = drivermatch.lookup(p);
  if(dr1 == -1 || (*p != ' ' && *p != '\r' && *p != '\n')) {
    // Currently incomplete, so skip
    while(*p != '\n')
      p++;
    p++;
    goto rerere;
    //    error(st, "Bad left driver name");
  }
  int dr2 = dr1;
  if(*p == ' ') {
    p++;
    dr2 = drivermatch.lookup(p);
    if(dr2 == -1 || (*p != '\r' && *p != '\n'))
      error(st, "Bad right driver name");
  }
  if(*p == '\r')
    p++;
  if(*p != '\n')
    error(st, "Crap after EOL");
  p++;
}
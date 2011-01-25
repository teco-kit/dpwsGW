#define debug_dom(label,top,stack)  _debug_dom(label,top,stack)
void _debug_dom(int num, void *x, void *y)
{
  ACLAddNewType(ACL_TYPE_ALPHA_ARG('S','T','E'));
  ACLAddData(num);
  ACLAddNewType(ACL_TYPE_ALPHA_ARG('P','T','R'));
  ACLAddData(hi((uint16_t)x));
  ACLAddData(lo((uint16_t)x));
  ACLAddData(hi((uint16_t)y));
  ACLAddData(lo((uint16_t)y));
  ACLSendPacket(10);
  while(ACLSendingBusy());
}

int dump(char *x, int len, long line)
{
  int i;
  ACLAddNewType(ACL_TYPE_ALPHA_ARG('B','I','N'));
  for(i=0;i<=((len-1)/8);i++)
  ACLAddData(x[i]);
  ACLAddNewType(ACL_TYPE_ALPHA_ARG('L','E','N'));
  ACLAddData(len);
  ACLAddData(line);
  ACLSendPacket(10);
  while(ACLSendingBusy());
  return 0;
}

int  _write_bits  (struct WRITER_STRUCT *writer, unsigned char *bits, int bits_len)
{
return  write_bits  (writer, bits, bits_len);
}

#undef write_true
#define write_true(X)  (dump(&__true,1,__LINE__),_write_bits(X,&__true, 1))

#undef write_false
#define write_false(X) (dump(&__false,1,__LINE__),_write_bits(X,&__false, 1))

#define write_bits(X,Y,Z) (dump(Y,Z,__LINE__),_write_bits(X,Y,Z))

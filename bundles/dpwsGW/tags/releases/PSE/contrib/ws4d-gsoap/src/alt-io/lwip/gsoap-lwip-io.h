#ifndef GSOAPLWIPIO_H_
#define GSOAPLWIPIO_H_

int gsoap_lwip_init (struct netif *netifp);

int gsoap_lwip_register_handle (struct soap *soap);

#endif /*GSOAPLWIPIO_H_ */

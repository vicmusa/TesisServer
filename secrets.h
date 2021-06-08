#include <pgmspace.h>

#define SECRET
#define THINGNAME "Lora32_Demo"                                      
                       
const char AWS_IOT_ENDPOINT[] = "a2gw59t2hmw6er-ats.iot.us-east-2.amazonaws.com";        //change this

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUDcmeAEPpAdmxN416Jh2WvpYzQ6QwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIxMDUxODE5Mzc0
M1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAJqoYm11w960iVgtvKIb
1MoJl+WCRNnV+vSr1uRHV/CeLIu+vhOOoCavK5FNpVAgPqHnzCQvyFH7glAl5Q9n
qhY6XVUGQbg9meFaQz054Da7krGvPomfYa/gsTFq6j5JosYhGuKE2r5XU/+4ol4J
h/R3XLnzgmI601+mmQa/75R4uMsLuPAP30MaB+uJ+5RIoDQdjRpVPAU8RbeqxhyB
9NpL8EICID2amBXUMgWIsKx+e2bXO85lH/KhK7PtGS1YORlZbok1NC/AYTZfWYpL
OfXQKBaGFYEa52r7toWfKUm8cKM2n4Y5QwEFZ1YE5WagmP2Ki3OaEI7xr5F6SFCo
I2kCAwEAAaNgMF4wHwYDVR0jBBgwFoAU3VyH1TLNEPPYpesG5tnnuY00D5cwHQYD
VR0OBBYEFDUVRj3Ss3WdwJy35F3UlQzZoRZjMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAMc09QDbvYRar/wLg/whRf9KfZ
oBIBGqBz59XDOslskoim/5NTvsNMqmXMRwMHWohdnkuxHRYMMtJwfIiinrO4PsJ4
IH/Yy5jQ7NIYoZ5o41DI+nRyQQhysFauLAxHIH6ndaVfd6mEXgJ7pKx1RvaBMtmh
2T86+sI69S0jgwDFQogEv7bU88mA/mbtRVXdeZ3n8rbyeF7Fby54YTpjtmfrTp6b
gfrXHbTk2GCrxaQcPzYCc4bQW3fwCaSjxxUo7Ncom+Rxc+qisgXqapVEepsNkR7K
b9z0MyzFrvvy/1xg6Cp6F1pTLjYNhm9WckvRIWf/DjZoa+Wxz92pEz9jcIvF
-----END CERTIFICATE-----
)KEY";

// Device Private Key                                               
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEogIBAAKCAQEAmqhibXXD3rSJWC28ohvUygmX5YJE2dX69KvW5EdX8J4si76+
E46gJq8rkU2lUCA+oefMJC/IUfuCUCXlD2eqFjpdVQZBuD2Z4VpDPTngNruSsa8+
iZ9hr+CxMWrqPkmixiEa4oTavldT/7iiXgmH9HdcufOCYjrTX6aZBr/vlHi4ywu4
8A/fQxoH64n7lEigNB2NGlU8BTxFt6rGHIH02kvwQgIgPZqYFdQyBYiwrH57Ztc7
zmUf8qErs+0ZLVg5GVluiTU0L8BhNl9Ziks59dAoFoYVgRrnavu2hZ8pSbxwozaf
hjlDAQVnVgTlZqCY/YqLc5oQjvGvkXpIUKgjaQIDAQABAoIBADu6V/jqCDEUDA7P
6w+4K9+zAMSUIL74Eqk3TBBxNRcqHw4BJisrY9JMyEifcf6V1Vp1yLMY+QYgWd1W
JTZj1J4tACp79hmPQMR1NY6JsXuxyR9yDHbd5L4lyojJP5m6mnBmDJn6sPfdhgU2
aw5iI+1Xw6nUzz/cdBF2p5ahvYba1swJ4pFqiiI5V/Ylsc2XSPNU7Nr/BRH7RK1u
XIrf5eyGASYIpiXoBXN2XUqAdnC0AZ24PSAAewBH6y3DLxt/0WdKKjcKE+2g7lux
1H5HVvwv+UrJyLbgwdQLzKq2ev6qAlV+oCgl2rrDzgA6ai77rJzHPfpz6eUVhXEs
uYxqxOECgYEAzcg/cDIlz8xhGfr5sNJpPSlx5f6vYoxwC5slPYwgyaqUsPT830al
KyQ7Zx/ihqPaFeZlDWJEZ5k6pshG6a8I2B19SDdA8cbd1/1XjSbCNdzBJA2iO9dN
2n8duAkHmPavvh07tEtfpNBg0DE2oAjhU0+iVbBNolUU/wLXLf4rUT0CgYEAwGZC
Av+wNaYQW6XHqOQMgz2B9WVIgvEOguOLuU3Cr3XpZg8+IuYGcd1ZbV5wm6tcavyS
18dIzd2HLK8j3EixhYbWsVz535KwnFARz1dWtqpa4V/dUj4bNXPUwytrynORY/qC
uIHvaoS66mganJSHdsqIZaX800fjBB3l+R1XpZ0CgYA8uFvPtVxm5n24ds/6lvuq
qnILVY1W3O2q3wqsEI8UMptRY9jB4UyXYEeO9F+ZLvwI/kTPIDHjB9nAoNQb3CQV
8Q2oo3UsYmvPcdfFtKwVzJ/JDaTTV4KHeH7CcRem0ROmW2Xkm2ELdtk6VWsy6avj
M1pJQo6F2ekqHiJ0Vc1eWQKBgAY+i1/rin2DQT6oN72sgRd1VJVxTxQo3K600eEW
u/3T1tpgHkUlsAncBD3/daonup0iS2XDawJ7cRqrKH/KwDA0Ox2Qhe+SpDa+hoVz
MTZGeyKbaOR1bY6jjsEy1JHChHJ5O+rfFLUhT6ux60MvsuHRGvIpm/YDfEOy8LbQ
E/slAoGAUngdBI8T4iG9ajj6lBCtb0EsUKu/AcSTwn/SrLO9sL4qi3PYRAOHI4Yq
+Pa1pZHcb/MxAqvmRGImmbmJaCoWQTVxBmOZlECB5HDFI58Sjfok5aPlhtugHWsn
b3p547uZLmAOIX2tmT1mt8Skf2eXQtNJ9TIyvubJ7eaKsna9Weg=
-----END RSA PRIVATE KEY-----
)KEY";

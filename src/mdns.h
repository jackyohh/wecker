#include <ESP8266mDNS.h>  // nur LEAmDNS funktioniert
MDNSResponder mdns;

#define SERVICE_PORT                                    80                                  // HTTP port
char*                                          pcHostDomain            = 0;        // Negociated host domain
bool                                           bHostDomainConfirmed    = false;    // Flags the confirmation of the host domain
MDNSResponder::hMDNSService                    hMDNSService            = 0;        // The handle of the http service in the MDNS responder
MDNSResponder::hMDNSServiceQuery               hMDNSServiceQuery       = 0;        // The handle of the 'http.tcp' service query in the MDNS responder

const String                                   cstrNoHTTPServices      = "Currently no 'http.tcp' services in the local network!<br/>";
String                                         strHTTPServices         = cstrNoHTTPServices;

// // HTTP server at port 'SERVICE_PORT' will respond to HTTP requests
// ESP8266WebServer                                     server(SERVICE_PORT);

bool setStationHostname(const char* p_pcHostname) {

  if (p_pcHostname) {
    WiFi.hostname(p_pcHostname);
    Serial.printf("setStationHostname: Station hostname is set to '%s'\n", p_pcHostname);
    return true;
  }
  return false;
}


void MDNSServiceQueryCallback(MDNSResponder::MDNSServiceInfo serviceInfo, MDNSResponder::AnswerType answerType, bool p_bSetContent) {
  String answerInfo;
  switch (answerType) {
    case MDNSResponder::AnswerType::ServiceDomain :
      answerInfo = "ServiceDomain " + String(serviceInfo.serviceDomain());
      break;
    case MDNSResponder::AnswerType::HostDomainAndPort :
      answerInfo = "HostDomainAndPort " + String(serviceInfo.hostDomain()) + ":" + String(serviceInfo.hostPort());
      break;
    case MDNSResponder::AnswerType::IP4Address :
      answerInfo = "IP4Address ";
      for (IPAddress ip : serviceInfo.IP4Adresses()) {
        answerInfo += "- " + ip.toString();
      };
      break;
    case MDNSResponder::AnswerType::Txt :
      answerInfo = "TXT " + String(serviceInfo.strKeyValue());
      for (auto kv : serviceInfo.keyValues()) {
        answerInfo += "\nkv : " + String(kv.first) + " : " + String(kv.second);
      }
      break;
    default :
      answerInfo = "Unknown Answertype";
  }
  // Serial.printf("Answer %s %s\n", answerInfo.c_str(), p_bSetContent ? "Modified" : "Deleted");
}

void serviceProbeResult(String p_pcServiceName,
                        const MDNSResponder::hMDNSService p_hMDNSService,
                        bool p_bProbeResult) {
  (void) p_hMDNSService;
  // Serial.printf("MDNSServiceProbeResultCallback: Service %s probe %s\n", p_pcServiceName.c_str(), (p_bProbeResult ? "succeeded." : "failed!"));
}

void hostProbeResult(String p_pcDomainName, bool p_bProbeResult) {

  Serial.printf("MDNSHostProbeResultCallback: Host domain '%s.local' is %s\n", p_pcDomainName.c_str(), (p_bProbeResult ? "free" : "already USED!"));

  if (true == p_bProbeResult) {
    // Set station hostname
    setStationHostname(pcHostDomain);

    if (!bHostDomainConfirmed) {
      // Hostname free -> setup clock service
      bHostDomainConfirmed = true;

      if (!hMDNSService) {
        // Add a 'http.tcp' service to port 'SERVICE_PORT', using the host domain as instance domain
        hMDNSService = MDNS.addService(0, "http", "tcp", SERVICE_PORT);
        if (hMDNSService) {
          MDNS.setServiceProbeResultCallback(hMDNSService, serviceProbeResult);

          // Add some '_http._tcp' protocol specific MDNS service TXT items
          // See: http://www.dns-sd.org/txtrecords.html#http
          MDNS.addServiceTxt(hMDNSService, "user", "");
          MDNS.addServiceTxt(hMDNSService, "password", "");
          MDNS.addServiceTxt(hMDNSService, "path", "/");
        }

        // Install dynamic 'http.tcp' service query
        if (!hMDNSServiceQuery) {
          hMDNSServiceQuery = MDNS.installServiceQuery("http", "tcp", MDNSServiceQueryCallback);
          if (hMDNSServiceQuery) {
            // Serial.printf("MDNSProbeResultCallback: Service query for 'http.tcp' services installed.\n");
          } else {
            Serial.printf("MDNSProbeResultCallback: FAILED to install service query for 'http.tcp' services!\n");
          }
        }
      }
    }
  } else {
    // Change hostname, use '-' as divider between base name and index
    if (MDNSResponder::indexDomain(pcHostDomain, "-", 0)) {
      MDNS.setHostname(pcHostDomain);
    } else {
      Serial.println("MDNSProbeResultCallback: FAILED to update hostname!");
    }
  }
}
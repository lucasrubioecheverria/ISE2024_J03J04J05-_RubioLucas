t <html><head><title>RTC</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("rtc.cgx", 500);
t function plotADGraph() {
t  horaFecha = document.getElementById("hora_fecha").value;
t  document.getElementById("hora_fecha").value=(horaFecha);
t }
t function periodicUpdateAd() {
t  if(document.getElementById("adChkBox").checked == true) {
t   updateMultiple(formUpdate,plotADGraph);
t   ad_elTime = setTimeout(periodicUpdateAd, formUpdate.period);
t  }
t  else
t   clearTimeout(ad_elTime);
t }
t </script></head>
i pg_header.inc
t <h2 align="center"><br>Reloj RTC: Real Time Clock</h2>
t <p><font size="2">En esta página se muestra el reloj RTC configurado inicialmente
t  en el día 27/02/2024 a las 12:00:00. Se podrá observar el reloj cada segundo.<br><br>
t </font></p>
t <form action="rtc.cgi" method="post" name="rtc">
t <input type="hidden" value="ad" name="pg">
t <table border=0 width=100%><font size="3">
t <tr style="background-color: #aaccff">
t  <th width=25%>          </th>
t  <th width=100%>HORA/FECHA</th>
t <tr><td><img src="pabb.gif">HORA/FECHA:</td>
t   <td align="center">
t <input type="text" readonly style="background-color: transparent; border: 0px"
c h 1  size="50" id="hora_fecha" value="%s"></td>
# t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
# c g 2  size="10" id="ad_volts" value="%5.3f V"></td>
# t <td height=50><table bgcolor="#FFFFFF" border="2" cellpadding="0" cellspacing="0" width="100%"><tr>
# c g 3 <td><table id="ad_table" style="width: %d%%" border="0" cellpadding="0" cellspacing="0">
# t <tr><td bgcolor="#0000FF">&nbsp;</td></tr></table></td></tr></table></td></tr>
t </font></table>
t <p align=center>
t <input type=button value="Refresh" onclick="updateMultiple(formUpdate,plotADGraph)">
t Periodic:<input type="checkbox" id="adChkBox" onclick="periodicUpdateAd()">
t </p></form>
i pg_footer.inc
. End of script must be closed with period

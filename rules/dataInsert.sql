SELECT 
current.state.reported.tempState AS tempState, current.state.reported.luxState AS luxState, current.state.reported.mode AS mode, 
timeStamp() AS timestamp, topic(3) AS thing_name, substring(topic(3), 11) AS sn, current.state.reported.mode AS mode 
FROM '$aws/things/+/shadow/update/documents' WHERE subString(topic(3), 0, 10) = 'greenhouse' AND current.state.reported.tempState >=0 AND 
current.state.reported.tempState <=1 AND current.state.reported.mode >= 0 AND current.state.reported.mode <= 1
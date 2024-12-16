SELECT current.state.reported.temperature AS temperature, timeStamp() 
AS timestamp, topic(3) AS thing_name 
FROM '$aws/things/+/shadow/update/documents' 
WHERE subString(topic(3), 0, 10) = 'greenhouse' 
AND current.state.reported.temperature >=0 
AND current.state.reported.temperature <=35



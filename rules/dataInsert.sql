SELECT 
    state.reported.tempState AS tempState, 
    state.reported.luxState AS luxState, 
    state.reported.mode AS mode, t
    imeStamp() AS timestamp, 
    topic(3) AS thing_name, 
    substring(topic(3), 11) AS sn
FROM '$aws/things/+/shadow/update/accepted'
WHERE subString(topic(3), 0, 10) = 'greenhouse' AND state.reported.tempState >=0 AND state.reported.tempState <=1

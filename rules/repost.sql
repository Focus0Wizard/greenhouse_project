SELECT 
    state.reported.tempState AS tempState, 
    state.reported.luxState AS luxState, 
    topic(3) AS thing_name
FROM '$aws/things/+/shadow/update/accepted'
WHERE 
    state.reported.tempState >= 0 AND 
    state.reported.tempState <= 1 AND 
    state.reported.luxState >= 0 AND 
    state.reported.luxState <= 1

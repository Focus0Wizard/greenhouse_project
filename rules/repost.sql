SELECT current.state.reported.tempState AS tempState, current.state.reported.luxState AS luxState, topic(3) AS thing_name,
current.state.reported.mode AS mode, current.state.reported.irrigation_state AS irrigation_state, 
current.state.reported.vent_state AS vent_state 
FROM '$aws/things/+/shadow/update/documents'
WHERE current.state.reported.tempState >= 0 
AND current.state.reported.tempState <= 1 
AND current.state.reported.luxState >= 0
AND current.state.reported.luxState <= 1 AND current.state.reported.mode = 0


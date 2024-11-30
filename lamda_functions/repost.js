import { IoTDataPlaneClient, UpdateThingShadowCommand, GetThingShadowCommand } from '@aws-sdk/client-iot-data-plane';
import { DynamoDBClient } from '@aws-sdk/client-dynamodb';
import { DynamoDBDocumentClient, ScanCommand } from '@aws-sdk/lib-dynamodb';

// Configuración de clientes
const iotClient = new IoTDataPlaneClient({ region: 'us-east-1' });
const dynamoDBClient = new DynamoDBClient({ region: 'us-east-1' });
const docClient = DynamoDBDocumentClient.from(dynamoDBClient);

// Función para obtener el thingName desde DynamoDB
async function getThingNameFromDynamoDB() {
    const params = {
        TableName: 'greenhouse_data',
        Limit: 1,
    };

    try {
        console.log("Obteniendo 'thingName' desde DynamoDB...");
        const command = new ScanCommand(params);
        const response = await docClient.send(command);

        if (response.Items && response.Items.length > 0) {
            console.log("'thingName' encontrado en DynamoDB:", response.Items[0].thing_name);
            return response.Items[0].thing_name;
        } else {
            throw new Error("No se encontró ningún 'thingName' en DynamoDB.");
        }
    } catch (error) {
        console.error('Error al obtener el thingName desde DynamoDB:', error);
        throw error;
    }
}

// Función para obtener el estado actual del Shadow
async function getThingShadowState(thingName) {
    const getParams = {
        thingName: thingName,
    };

    try {
        console.log(`Obteniendo estado del Shadow para el dispositivo: ${thingName}`);
        const command = new GetThingShadowCommand(getParams);
        const response = await iotClient.send(command);

        if (response.payload) {
            const payload = JSON.parse(new TextDecoder().decode(response.payload));
            console.log('Estado actual del Shadow:', payload.state.reported);
            return payload.state.reported;
        } else {
            throw new Error('No se pudo obtener el estado del Shadow.');
        }
    } catch (error) {
        console.error('Error al obtener el estado del Shadow:', error);
        throw error;
    }
}

// Función para actualizar el Shadow del dispositivo
async function updateThingShadow(thingName, irrigation_state, vent_state) {
    const shadowPayload = {
        state: {
            reported: {
                irrigation_state: irrigation_state,
                vent_state: vent_state,
            },
        },
    };

    const updateParams = {
        thingName: thingName,
        payload: JSON.stringify(shadowPayload),
    };

    try {
        console.log(`Actualizando Shadow para el dispositivo: ${thingName}`);
        const command = new UpdateThingShadowCommand(updateParams);
        const response = await iotClient.send(command);

        if (response.payload) {
            console.log('Shadow actualizado exitosamente:', JSON.parse(new TextDecoder().decode(response.payload)));
            return JSON.parse(new TextDecoder().decode(response.payload));
        } else {
            throw new Error('Error al actualizar el Shadow.');
        }
    } catch (error) {
        console.error('Error al actualizar el Shadow:', error);
        throw error;
    }
}

// Lambda handler
export const handler = async (event) => {
    try {
        console.log('Evento recibido:', JSON.stringify(event, null, 2));

        // Obtener el thingName desde el evento o DynamoDB
        let thingName = event['thing_name'];
        if (!thingName) {
            console.log("No se encontró 'thingName' en el evento. Intentando obtener desde DynamoDB...");
            thingName = await getThingNameFromDynamoDB();
        }

        // Extraer valores desde el evento
        let tempState = null;
        let luxState = null;

        // Intentar diferentes rutas para encontrar los valores
        if (event.state?.reported) {
            tempState = event.state.reported.tempState;
            luxState = event.state.reported.luxState;
        } else if (event.tempState !== undefined && event.luxState !== undefined) {
            tempState = event.tempState;
            luxState = event.luxState;
        }

        if (tempState === null || luxState === null) {
            console.error("No se encontraron 'tempState' o 'luxState' en el evento:", JSON.stringify(event));
            throw new Error("Faltan valores de 'tempState' o 'luxState' en el evento.");
        }

        console.log(`Valores extraídos: tempState=${tempState}, luxState=${luxState}`);

        // Calcular los nuevos estados
        const irrigation_state = tempState === 1 ? 1 : 0;
        const vent_state = luxState === 1 ? 1 : 0;

        console.log(`Estados calculados: irrigation_state=${irrigation_state}, vent_state=${vent_state}`);

        // Obtener el estado actual del Shadow
        const currentShadowState = await getThingShadowState(thingName);

        // Verificar si los valores han cambiado antes de actualizar el Shadow
        if (
            currentShadowState.irrigation_state === irrigation_state &&
            currentShadowState.vent_state === vent_state
        ) {
            console.log("No es necesario actualizar el Shadow. Los estados son los mismos.");
            return {
                statusCode: 200,
                body: JSON.stringify({ message: "El Shadow ya está actualizado." }),
            };
        }

        // Actualizar el Shadow con los nuevos valores
        const updatedShadow = await updateThingShadow(thingName, irrigation_state, vent_state);

        return {
            statusCode: 200,
            body: JSON.stringify({ message: "Shadow actualizado correctamente.", updatedShadow }),
        };
    } catch (error) {
        console.error('Error en la función Lambda:', error);
        return {
            statusCode: 500,
            body: JSON.stringify({ message: "Error procesando el evento.", error: error.message }),
        };
    }
};
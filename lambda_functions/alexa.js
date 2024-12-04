const Alexa = require('ask-sdk-core');
const AWS = require('aws-sdk');
const IotData = new AWS.IotData({ endpoint: 'abkb9nclg5hh-ats.iot.us-east-1.amazonaws.com' });
const dynamoDb = new AWS.DynamoDB.DocumentClient();

async function getThingName(userId) {
    const params = {
        TableName: 'thingUser',
        Key: { UserId: userId }
    };

    try {
        const result = await dynamoDb.get(params).promise();
        return result.Item ? result.Item.thing_name : null;
    } catch (error) {
        console.error('Error al obtener el thingName de DynamoDB:', error);
        throw new Error('No se pudo obtener el thingName');
    }
}

function logUserId(handlerInput) {
    const userId = handlerInput.requestEnvelope.context.System.user.userId;
    console.log(`UserId: ${userId}`);  // Registra el userId en CloudWatch logs
    return userId;  // Puedes retornar el userId si necesitas usarlo más tarde
}

async function getShadowParams(userId) {
    const thingName = await getThingName(userId);
    console.log(thingName);
    if (!thingName) {
        throw new Error('Thing name no encontrado');
    }
    return { thingName: thingName };
}

function getShadowPromise(params) {
    return new Promise((resolve, reject) => {
        IotData.getThingShadow(params, (err, data) => {
            if (err) {
                console.log(err, err.stack);
                reject('Error al obtener el shadow: ' + err.message);
            } else {
                resolve(JSON.parse(data.payload));
            }
        });
    });
}

// Function to update the desired state of the Shadow
function updateShadowPromise(params) {
    return new Promise((resolve, reject) => {
        IotData.updateThingShadow(params, (err, data) => {
            if (err) {
                console.log(err, err.stack);
                reject('Error al actualizar el shadow: ' + err.message);
            } else {
                resolve(data);
            }
        });
    });
}

// LaunchRequest Handler
const LaunchRequestHandler = {
  canHandle(handlerInput) {
      const userId = handlerInput.requestEnvelope.context.System.user.userId;
      return Alexa.getRequestType(handlerInput.requestEnvelope) === 'LaunchRequest';
  },
  handle(handlerInput) {
      const speakOutput = 'Bienvenido al invernadero inteligente. Puedes preguntar por el estado de temperatura o cambiar algun estado de los actuadores. ¿Qué deseas hacer?';
      return handlerInput.responseBuilder.speak(speakOutput).reprompt(speakOutput).getResponse();
  }
};

// Handler para consultar el estado de la temperatura
const CheckTemperatureStateHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'tempStatusIntent';
    },
    async handle(handlerInput) {
        let speakOutput = 'No se pudo obtener el estado de la temperatura.';
        try {
            const userId = handlerInput.requestEnvelope.context.System.user.userId;
            const ShadowParams = await getShadowParams(userId);
            const result = await getShadowPromise(ShadowParams);
            const tempState = result.state.reported.tempState;

            switch (tempState) {
                case 0:
                    speakOutput = 'La temperatura está fría/media.';
                    break;
                case 1:
                    speakOutput = 'La temperatura está en estado ideal.';
                    break;
                default:
                    speakOutput = 'No se pudo determinar el estado de la temperatura.';
            }
        } catch (error) {
            console.log(error);
        }

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const CheckLuxStateHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'luxStateIntent';
    },
    async handle(handlerInput) {
        let speakOutput = 'No se pudo obtener el estado de la luz.';
        try {
            const userId = handlerInput.requestEnvelope.context.System.user.userId;
            const ShadowParams = await getShadowParams(userId);
            const result = await getShadowPromise(ShadowParams);
    
            console.log('Contenido del Shadow:', JSON.stringify(result)); // Log para depurar
    
            const luxState = result.state?.reported?.luxState; // Manejo seguro de atributos
    
            switch (luxState) {
                case 0:
                    speakOutput = 'La luz está en un nivel medio.';
                    break;
                case 1:
                    speakOutput = 'La luz está en un nivel alto.';
                    break;
                default:
                    speakOutput = 'No se pudo determinar el estado de la luz.';
            }
        } catch (error) {
            console.error('Error en luxStatusIntent:', error);
        }
    
        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }    
};

const CheckHouseModeStateHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'CheckIncubatorModeStateIntent';
    },
    async handle(handlerInput) {
        let speakOutput = 'No se pudo obtener el estado de el invernadero.';
        try {
            const userId = handlerInput.requestEnvelope.context.System.user.userId;
            const ShadowParams = await getShadowParams(userId);
            const result = await getShadowPromise(ShadowParams);
            const incubatorM = result.state.reported.mode;

            switch (incubatorM) {
                case 0:
                    speakOutput = 'El invernadero está en modo automatico.';
                    break;
                case 1:
                    speakOutput = 'El invernadero esta en modo manual.';
                    break;
                default:
                    speakOutput = 'No se pudo determinar el estado de el invernadero.';
            }
        } catch (error) {
            console.log(error);
        }

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const CheckIrrigationStateHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'irigationStateIntent';
    },
    async handle(handlerInput) {
        let speakOutput = 'No se pudo obtener el estado de riego.';
        try {
            const userId = handlerInput.requestEnvelope.context.System.user.userId;
            const ShadowParams = await getShadowParams(userId);
            const result = await getShadowPromise(ShadowParams);
            const coolerS = result.state.reported.irrigation_state;

            switch (coolerS) {
                case 0:
                    speakOutput = 'El regado esta apagado';
                    break;
                case 1:
                    speakOutput = 'El regado esta encendido.';
                    break;
                default:
                    speakOutput = 'No se pudo determinar el estado de la regadora.';
            }
        } catch (error) {
            console.log(error);
        }

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const CheckBlindStateHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'blindStateIntent';
    },
    async handle(handlerInput) {
        let speakOutput = 'No se pudo obtener el estado de las persianas.';
        try {
            const userId = handlerInput.requestEnvelope.context.System.user.userId;
            const ShadowParams = await getShadowParams(userId);
            const result = await getShadowPromise(ShadowParams);
            const lightS = result.state.reported.vent_state;

            switch (lightS) {
                case 0:
                    speakOutput = 'El sistema de persianas esta apagado';
                    break;
                case 1:
                    speakOutput = 'El sistema de persianas esta prendido.';
                    break;
                default:
                    speakOutput = 'No se pudo determinar el estado de las persianas.';
            }
        } catch (error) {
            console.log(error);
        }

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const ChangeBlindStateManualHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'setBlindIntent';
    },
    async handle(handlerInput) {
        let open;
        const status = handlerInput.requestEnvelope.request.intent.slots.blindSlot.value;

        if (status === "abrir") { 
            open = 1; 
        } 
        if(status === "cerrar"){
            open = 0
        }

        try {
            const userId = handlerInput.requestEnvelope.context.System.user.userId;
            const ShadowParams = await getShadowParams(userId);

            const payload = {
                state: {
                    reported: {
                        luxState: open,
                        vent_state: open ,
                        mode: 1 
                    }
                }
            };

            const params = {
                ...ShadowParams,
                payload: JSON.stringify(payload)
            };

            await updateShadowPromise(params);
            const speakOutput = `las persianas funcionaron`;
            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        } catch (error) {
            console.error(error);
            return handlerInput.responseBuilder
                .speak('Hubo un error al cambiar el estado de las persianas. Por favor, inténtalo de nuevo.')
                .getResponse();
        }
    }
};


const ChangeIrigationStateManualHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'setIrigationIntent';
    },
    async handle(handlerInput) {
        let open;
        const status = handlerInput.requestEnvelope.request.intent.slots.irigationSlot.value;

        if (status === "abrir") { 
            open = 1; 
        } 
        if(status === "cerrar"){
            open = 0
        }

        try {
            const userId = handlerInput.requestEnvelope.context.System.user.userId;
            const ShadowParams = await getShadowParams(userId);

            const payload = {
                state: {
                    reported: {
                        tempState: open,
                        irrigation_state: open,
                        mode: 1 
                    }
                }
            };

            const params = {
                ...ShadowParams,
                payload: JSON.stringify(payload)
            };

            await updateShadowPromise(params);
            const speakOutput = `el sistema de riego funcionó`;
            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        } catch (error) {
            console.error(error);
            return handlerInput.responseBuilder
                .speak('Hubo un error al cambiar el estado del sistema de riego. Por favor, inténtalo de nuevo.')
                .getResponse();
        }
    }
};


// Handler para cambiar el modo a automático
const ChangeAutoModeHandler = {
    canHandle(handlerInput) {
        const requestType = Alexa.getRequestType(handlerInput.requestEnvelope);
        const intentName = Alexa.getIntentName(handlerInput.requestEnvelope);
        console.log(`RequestType: ${requestType}, IntentName: ${intentName}`);
        return requestType === 'IntentRequest' && intentName === 'changeModeIntent';
    },
    async handle(handlerInput) {
        const userId = handlerInput.requestEnvelope.context.System.user.userId;
        const ShadowParams = await getShadowParams(userId);

        const payload = {
            state: {
                reported: {
                    mode: 0
                }
            }
        };

        const params = {
            ...ShadowParams,
            payload: JSON.stringify(payload)
        };

        try {
            await updateShadowPromise(params);
            const speakOutput = 'El modo automático se ha activado.';
            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        } catch (error) {
            console.log(error);
            return handlerInput.responseBuilder
                .speak('Hubo un error al activar el modo automático.')
                .getResponse();
        }
    }
};


// Otros handlers estándar
const HelpIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.HelpIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Puedes pedirme consultar o cambiar el estado de la temperatura y el servomotor. ¿Qué deseas hacer?';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const CancelAndStopIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && (Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.CancelIntent'
                || Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.StopIntent');
    },
    handle(handlerInput) {
        const speakOutput = '¡Hasta luego!';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .getResponse();
    }
};

const ErrorHandler = {
    canHandle() {
        return true;
    },
    handle(handlerInput, error) {
        console.log(`Error: ${error.message}`);
        return handlerInput.responseBuilder
            .speak('Hubo un problema. Intenta de nuevo.')
            .getResponse();
    }
};

// Exporta la skill con todos los handlers
exports.handler = Alexa.SkillBuilders.custom()
    .addRequestHandlers(
        LaunchRequestHandler,
        CheckTemperatureStateHandler,
        CheckLuxStateHandler,
        HelpIntentHandler,
        CancelAndStopIntentHandler,
        CheckHouseModeStateHandler,
        CheckIrrigationStateHandler,
        CheckBlindStateHandler,
        ChangeIrigationStateManualHandler,
        ChangeBlindStateManualHandler,
        ChangeAutoModeHandler
    )
    .addErrorHandlers(ErrorHandler)
    .lambda();
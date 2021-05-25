# TÍTULO

A parte de estos contenidos, apartados libres

## Tamble of contents 

* Intro 
  * El proyecto
  * Qué és un potencionstato
  * En que consisten las medidas, y para qué sirven?
* Objetivos



## INTRO

Introduir el ViSense i el porotocol de comuncicaió

-en qué consiste el proyecto

El proyecto consiste en la programación de un potenciostato a partir de la EV Nucleo-F401R de STMicroelectroncs. La finalidad de este és la caracterización de la concentración de varias disoluciones de Ferricianuro de potasio en un tampón de cloruro de potasio por medio del potenciostato.

*-qué es un potenciostato*

Un potenciostato es un dispositivo que mide y amplifica una señal proveniente de una reacción electroquímica, permitiendo medir las concentraciones de un analito 

Un  potenciostato cualquiera puede medir en modo amperometrico, donde se fija el potencial y se mira la corriente de salida, o potenciometrico (volumétrico), donde se fija el corriente y se mira el voltage.  En este poryecto estamos trabajando en modo Amperometrico, fijando Vcell y midiendo Icell. 

Las configuraciones típicas para sensores amperométricos, que miden la corriente que sale de una reacción electroquímica, consisten en un enfoque de dos electrodos, donde hay un electrodo de trabajo donde tiene lugar la reacción y un electrodo de referencia junto con un electrodo auxiliar, que rastrean el potencial resultante de la reacción y proporcionan la corriente requerida. Sin embargo, este enfoque presenta un problema que se basa en la acumulación de cargas en el electrodo de referencia, que se supone que tiene un potencial bien conocido. Para superar este problema, se define la configuración de tres electrodos.

* Electrodo de trabajo (WE): el electrodo de trabajo es aquel donde se produce la reacción Electroquímica, cambiando la tensión del electrodo.
* Electrodo de referencia (RE): se utiliza para medir cambios en el potencial del electrodo de trabajo.
* Electrodo auxiliar (AE):Suministra la corriente necesaria para la reacción electroquímica en el electrodo de trabajo.



-en qué consisten las medidas electroquímicas hechas y para qué sirven

En el proyecto hemos programado dos tipos de medidas diferentes, La voltometria Cilcica (CV) i la cronoamperometria (CA)

*Voltammetría cíclica
Una Voltammetría Cíclica (CV) es un tipo de medición electroquímica potenciodinámica en la que se aplica un potencial variable a una celda electroquímica mientras se mide la corriente que esta celda proporciona. El potencial entre el electrodo de trabajo (WE) y el de referencia (RE) de la celda varía con el tiempo hasta que alcanza un valor potencial establecido, luego cambia de dirección, realizando lo que se denomina barrido triangular de potencial. Este proceso se repite durante un número establecido de ciclos. El resultado se representa en un voltamograma cíclico, que representa la corriente a través de la celda frente al voltaje aplicado en esta. La CV es una técnica ampliamente utilizada para estudiar las propiedades electroquímicas de un analito en una solución. Una CV proporciona gran cantidad de información sobre el comportamiento químico y físico de un sistema. Además, se pueden observar diferentes fenómenos físicos realizando voltammetrías a diferentes velocidades de exploración (modificando la velocidad de cambio de voltaje con el tiempo).*

*Cronoamperometría
Una Cronoamperometría (CA) es una técnica electroquímica que transduce la actividad de las especies biológicas de una celda electroquímica en una señal de corriente que cuantifica la concentración del analito de interés. En esta técnica se aplica una señal escalón y se mide la corriente a través de la celda en función del tiempo. Una de las ventajas de esta técnica es que no requiere etiquetado de analito o biorreceptor. El experimento comienza manteniendo la celda a un potencial en el que no ocurre ningún proceso faradaico. Entonces, el potencial se eleva a un valor en el cual ocurre una reacción redox.*

## Objetivos

* Programar un potenciostato portable
* Controlar la Power Management Unit del modulo front endo del potenciostato (REVISAR)
* Comunicarse con la aplicación ViSense 
* Implementar una Voltometría Cíclica
* Implementar una Crono Amperometria

## METODOLOGÍA

-diagrama de flujo

-explicación de la aplicación final

## RESULTADOS



## CONCLUSIONES

N
# Data Structures - Linked Lists

Este documento describe la jerarquía de estructuras de listas enlazadas implementadas en el proyecto.

---

## Class Diagram

```mermaid
classDiagram

LinkedList <|-- DoubleLinkedList
LinkedList <|-- CircularLinkedList
DoubleLinkedList <|-- CircularDoubleLinkedList
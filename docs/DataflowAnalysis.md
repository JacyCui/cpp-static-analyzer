# Dataflow Analysis

> You should first read the tutorial about program representation before proceeding to this guide.



## High Level Overview of an Analysis

An analysis is an algorithm that takes a program and some configuration as input and returns some results. Typically, results are bound to statements.

```mermaid
---
title: Top Interfaces for an Analsis
---
classDiagram
class Analysis {
    <<interface>>
    +getAnalysisConfig() AnalysisConfig
}

class AnalysisConfig {
	<<interface>>
	+getDescription() String
}

note for MethodAnalysis "R is a generic result type of some analysis"

class MethodAnalysis {
	<<interface>>
	+analyze(IR): R
}

Analysis <|-- MethodAnalysis
Analysis o-- AnalysisConfig

note for StmtResult "R is a generic result type for a single statement"

class StmtResult {
	<<interface>>
	+isRelevant(Stmt) bool
	+getResult(Stmt) R
}

MethodAnalysis .. StmtResult
```




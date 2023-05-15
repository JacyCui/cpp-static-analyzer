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
    <<abstract>>
    +getAnalysisConfig() AnalysisConfig
}

class AnalysisConfig {
	<<interface>>
	+getDescription()* String
}

note for MethodAnalysis "R is a generic result type of some analysis"

class MethodAnalysis~R~ {
	<<abstract>>
	+analyze(IR)* R
}

Analysis <|-- MethodAnalysis
Analysis o-- AnalysisConfig

note for StmtResult "R is a generic result type for a single statement"

class StmtResult~R~ {
	<<interface>>
	+isRelevant(Stmt)* bool
	+getResult(Stmt)* R
}

MethodAnalysis --> StmtResult
```

You can easily create your own classes to override these interfaces in order to develop your own analysis.



## Dataflow Analysis Framework

This project provide a complete and easy-to-use framework to develop dataflow analysis.

```mermaid
---
title: Dataflow Analysis Framework
---
classDiagram
class MethodAnalysis~R~ {
	<<abstract>>
	+analyze(IR)* R
}

class AnalysisDriver {
	<<abstract>>
	+analyze(IR) DataflowResult
	#makeAnalysis()* DataflowAnalysis
}

MethodAnalysis <|-- AnalysisDriver 
AnalysisDriver --> Solver
AnalysisDriver --> DataflowAnalysis
AnalysisDriver --> DataflowResult

class Solver {
	<<interface>>
	+solve(DataflowAnalysis)* DataflowResult
}

class AbstractSolver {
	<<abstract>>
	+solve(DataflowAnalysis) DataflowResult
	#initializeForward(DataflowAnalysis, DataflowResult)
	#initializeBackward(DataflowAnalysis, DataflowResult)
	#doSolveForward(DataflowAnalysis, DataflowResult)*
	#doSolveBackward(DataflowAnalysis, DataflowResult)*
}

Solver <|-- AbstractSolver

class DataflowAnalysis {
	<<interface>>
	+isForward()* bool
	+newBoundaryFact()* Fact
	+newInitialFact()* Fact
	+meetInto(Fact fact, Fact target)*
	+transferNode(Stmt, Fact in, Fact out)* bool
	+getCFG() CFG
}

NodeResult~Fact~ <|-- DataflowResult~Fact~

class NodeResult~Fact~ {
	+getInFact(Stmt)* Fact
	+getOutFact(Stmt)* Fact
}

class DataflowResult~Fact~ {
	+getInFact(Stmt) Fact
	+getOutFact(Stmt) Fact
	+setInFact(Stmt) Fact
	+setOutFact(Stmt) Fact
}
```




# PtahStale

> Procedural RPG baseado na **7ª Edição de Call of Cthulhu**, desenvolvido com **Unreal Engine 5.4.4**.

---

## Visão Geral

Ptah's Tale é um jogo de investigação e horror cósmico de mundo aberto procedural. Cada partida gera um novo mapa, locações, pistas e encontros inspirados no universo Lovecraftiano, seguindo as mecânicas oficiais da **7ª Edição do RPG Call of Cthulhu** da Chaosium.

O projeto é construído em **C++ + Blueprint** sobre Unreal Engine 5.4.4, aproveitando:
- **PCG (Procedural Content Generation) Framework** para geração de mundos
- **Enhanced Input** para controles modernos
- **Lumen + Nanite** para renderização de alta fidelidade

---

## Estrutura do Projeto

```
PtahStale/
├── PtahStale.uproject          ← Arquivo de projeto UE 5.4.4
├── Source/PtahStale/
│   ├── PtahStale.h/.cpp        ← Módulo principal do jogo
│   ├── PtahStaleGameMode.h/.cpp
│   ├── PtahStaleCharacter.h/.cpp ← Investigador controlado pelo jogador
│   ├── CoC/
│   │   ├── CoCCharacterStats   ← 8 Características + Stats Derivados (CoC 7e)
│   │   ├── SanitySystem        ← Sistema completo de Sanidade (CoC 7e)
│   │   └── SkillSystem         ← Lista de perícias + rolagens + evolução
│   └── Procedural/
│       └── ProceduralWorldGenerator ← Geração procedural de locações e salas
├── Config/
│   ├── DefaultGame.ini
│   ├── DefaultEngine.ini
│   └── DefaultInput.ini
└── Content/                    ← Assets gerenciados pelo Unreal Editor
```

---

## Mecânicas Implementadas (C++)

### Características do Investigador (CoC 7e)
As **8 Características Fundamentais** estão implementadas em `CoCCharacterStats`:

| Característica | Abreviação |
|---|---|
| Strength (Força) | STR |
| Constitution (Constituição) | CON |
| Size (Tamanho) | SIZ |
| Dexterity (Destreza) | DEX |
| Appearance (Aparência) | APP |
| Intelligence (Inteligência) | INT |
| Power (Poder) | POW |
| Education (Educação) | EDU |

**Stats Derivados** calculados automaticamente:
- **Hit Points** = (CON + SIZ) / 10
- **Magic Points** = POW / 5
- **Sanity (máx.)** = POW
- **Move Rate** (8 base, modificado por idade e stats)
- **Damage Bonus / Build** (tabela CoC 7e)

### Sistema de Sanidade (`SanitySystem`)
- Perda de sanidade com rolagem de **POW** (sucesso = perda mínima)
- **Insanidade Temporária**: perde ≥ 5 SAN em uma única rodada → Bout of Madness
- **Insanidade Indefinida**: perde ≥ 1/5 da SAN máxima em uma sessão
- **Insanidade Permanente**: SAN chega a 0
- **Recuperação** por terapia ou recompensas
- Cap de SAN máxima por conhecimento de **Cthulhu Mythos** (99 − Mythos)

### Sistema de Perícias (`SkillSystem`)
- **40+ perícias** pré-carregadas com valores base do livro (7ª Ed.)
- Rolagens em **3 níveis**: Regular / Hard (½) / Extreme (1/5)
- **Marcação de experiência** e **rolagens de evolução** ao fim do cenário

### Gerador de Mundo Procedural (`ProceduralWorldGenerator`)
- Algoritmo de expansão **BSP-inspired** com grid de salas
- Semente determinística (`Seed`) para reprodutibilidade
- Tags de locação: Town, Countryside, Coastal, Forest, Ruins, Underground, **Deeply Wrong**
- Distribuição aleatória de **pistas Mythos** e **encontros**
- Saídas compatíveis com **PCG Framework** do UE5

---

## Requisitos

- **Unreal Engine 5.4.4**
- **Visual Studio 2022** (Windows) ou **Xcode 15** (macOS)
- Plugins habilitados: `PCG`, `ModelingToolsEditorMode`

---

## Como Abrir o Projeto

1. Instale o Unreal Engine 5.4.4 via Epic Games Launcher
2. Clone o repositório:
   ```bash
   git clone https://github.com/liljohw/ptahstale.git
   ```
3. Clique com o botão direito em `PtahStale.uproject` → **Generate Visual Studio project files**
4. Abra `PtahStale.sln` e compile na configuração `Development Editor`
5. Abra `PtahStale.uproject` no Unreal Editor

---

## Roadmap

- [ ] Gerador de NPCs procedurais com personalidade e segredos
- [ ] Sistema de combate baseado em turnos (opcional, estilo CoC)
- [ ] Criação de personagem em jogo (distribuição de pontos de ocupação/interesse pessoal)
- [ ] Cenários gerados proceduralmente com arcos narrativos
- [ ] Inventário e sistema de grimórios/tomos
- [ ] Integração com UE5 PCG para foliagem e props
- [ ] Sistema de audio atmosférico (Metasounds)
- [ ] Localização PT-BR / EN

---

## Licença

Projeto privado – todos os direitos reservados.

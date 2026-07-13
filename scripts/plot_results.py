import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import os
import re

def save_figure(filename, dpi=None):
    try:
        if dpi:
            plt.savefig(filename, dpi=dpi, bbox_inches='tight')
        else:
            plt.savefig(filename, bbox_inches='tight')
    except PermissionError:
        alt_filename = filename.replace('.pdf', '_new.pdf')
        print(f"\nAviso: Permissao negada ao salvar {filename}. Tentando {alt_filename}...")
        if dpi:
            plt.savefig(alt_filename, dpi=dpi, bbox_inches='tight')
        else:
            plt.savefig(alt_filename, bbox_inches='tight')
    plt.close()

def render_table(data, filename, title, subtitle=None):
    fig, ax = plt.subplots(figsize=(20, len(data) * 0.35 + 2.0))
    ax.axis('off')
    
    if subtitle:
        ax.set_title(title, fontweight='bold', size=16, pad=35)
        ax.text(0.5, 1.02, subtitle, transform=ax.transAxes, ha='center', va='bottom', fontsize=12, style='italic', color='#555555')
    else:
        ax.set_title(title, fontweight='bold', size=16, pad=20)
    
    table = ax.table(cellText=data.values, 
                     colLabels=data.columns, 
                     cellLoc='center', 
                     loc='center')
    
    table.auto_set_font_size(False)
    table.set_fontsize(10)
    table.scale(1, 1.5)
    
    for (row, col), cell in table.get_celld().items():
        if row == 0:
            cell.set_text_props(weight='bold', color='white')
            cell.set_facecolor('#4c72b0')
        else:
            cell.set_facecolor('#f5f5f5' if row % 2 == 0 else '#ffffff')

    plt.tight_layout()
    save_figure(filename, dpi=150)

def render_paper_table(data_df, filename, title, has_best=True):
    num_rows = len(data_df)
    
    if has_best:
        col_widths = [3.2, 1.0, 1.2, 1.0, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9]
    else:
        col_widths = [3.2, 1.2, 1.0, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9]
        
    total_width = sum(col_widths)
    row_height = 0.35
    header_height = 0.4
    
    x_positions = [0]
    for w in col_widths[:-1]:
        x_positions.append(x_positions[-1] + w)
        
    total_height = 2 * header_height + num_rows * row_height
    
    fig, ax = plt.subplots(figsize=(total_width * 0.9, total_height * 0.8))
    ax.axis('off')
    ax.set_xlim(0, total_width)
    ax.set_ylim(0, total_height)
    
    # Title
    ax.text(total_width / 2.0, total_height + 0.1, title, ha='center', va='bottom', weight='bold', fontsize=12)
    
    def draw_cell(x, y, w, h, text, is_header=False, align='center', bg_color=None, weight='normal', font_size=9):
        if bg_color is None:
            bg_color = '#ffffff'
        rect = plt.Rectangle((x, y), w, h, facecolor=bg_color, edgecolor='#bbbbbb', linewidth=0.8)
        ax.add_patch(rect)
        
        tx = x + w / 2.0 if align == 'center' else x + 0.15
        ty = y + h / 2.0
        ax.text(tx, ty, text, ha=align, va='center', weight=weight, color='black', fontsize=font_size)

    y = total_height - header_height
    
    # Header Row 0
    if has_best:
        draw_cell(x_positions[0], y - header_height, col_widths[0], 2 * header_height, 'Instância', is_header=True, font_size=10, weight='bold')
        draw_cell(x_positions[1], y - header_height, col_widths[1], 2 * header_height, 'Melhor', is_header=True, font_size=10, weight='bold')
        draw_cell(x_positions[2], y - header_height, col_widths[2], 2 * header_height, 'Literatura', is_header=True, font_size=10, weight='bold')
        draw_cell(x_positions[3], y - header_height, col_widths[3], 2 * header_height, 'Guloso', is_header=True, font_size=10, weight='bold')
        
        # Spanning headers
        draw_cell(x_positions[4], y, sum(col_widths[4:7]), header_height, 'Randomizado', is_header=True, font_size=10, weight='bold')
        draw_cell(x_positions[7], y, sum(col_widths[7:10]), header_height, 'Reativo', is_header=True, font_size=10, weight='bold')
        
        # Sub-headers (Row 1)
        y -= header_height
        draw_cell(x_positions[4], y, col_widths[4], header_height, '0,30', is_header=True, font_size=9, weight='bold')
        draw_cell(x_positions[5], y, col_widths[5], header_height, '0,50', is_header=True, font_size=9, weight='bold')
        draw_cell(x_positions[6], y, col_widths[6], header_height, '0,80', is_header=True, font_size=9, weight='bold')
        
        draw_cell(x_positions[7], y, col_widths[7], header_height, '0,50', is_header=True, font_size=9, weight='bold')
        draw_cell(x_positions[8], y, col_widths[8], header_height, '1,00', is_header=True, font_size=9, weight='bold')
        draw_cell(x_positions[9], y, col_widths[9], header_height, '2,00', is_header=True, font_size=9, weight='bold')
    else:
        draw_cell(x_positions[0], y - header_height, col_widths[0], 2 * header_height, 'Instância', is_header=True, font_size=10, weight='bold')
        draw_cell(x_positions[1], y - header_height, col_widths[1], 2 * header_height, 'Literatura', is_header=True, font_size=10, weight='bold')
        draw_cell(x_positions[2], y - header_height, col_widths[2], 2 * header_height, 'Guloso', is_header=True, font_size=10, weight='bold')
        
        # Spanning headers
        draw_cell(x_positions[3], y, sum(col_widths[3:6]), header_height, 'Randomizado', is_header=True, font_size=10, weight='bold')
        draw_cell(x_positions[6], y, sum(col_widths[6:9]), header_height, 'Reativo', is_header=True, font_size=10, weight='bold')
        
        # Sub-headers (Row 1)
        y -= header_height
        draw_cell(x_positions[3], y, col_widths[3], header_height, '0,30', is_header=True, font_size=9, weight='bold')
        draw_cell(x_positions[4], y, col_widths[4], header_height, '0,50', is_header=True, font_size=9, weight='bold')
        draw_cell(x_positions[5], y, col_widths[5], header_height, '0,80', is_header=True, font_size=9, weight='bold')
        
        draw_cell(x_positions[6], y, col_widths[6], header_height, '0,50', is_header=True, font_size=9, weight='bold')
        draw_cell(x_positions[7], y, col_widths[7], header_height, '1,00', is_header=True, font_size=9, weight='bold')
        draw_cell(x_positions[8], y, col_widths[8], header_height, '2,00', is_header=True, font_size=9, weight='bold')
        
    for r_idx, row in data_df.iterrows():
        y -= row_height
        
        is_media = (row['instance'] == 'Média')
        bg = '#f9f9f9' if (r_idx % 2 == 1 and not is_media) else '#ffffff'
        if is_media:
            bg = '#f1f1f1'
            
        algo_cols = ['greedy', 'rand_0.3', 'rand_0.5', 'rand_0.8', 'react_0.5', 'react_1.0', 'react_2.0']
        if not is_media:
            row_algo_vals = [row[c] for c in algo_cols if pd.notna(row[c])]
            min_val = min(row_algo_vals) if row_algo_vals else None
        else:
            min_val = None
            
        for c_idx, col_name in enumerate(data_df.columns):
            val = row[col_name]
            
            if col_name == 'instance':
                display_text = str(val).replace('scenario_', '')
                font_weight = 'bold' if is_media else 'normal'
                align = 'left'
            elif col_name == 'best':
                display_text = f"{val:.2f}".replace('.', ',') if pd.notna(val) else ""
                font_weight = 'bold' if is_media else 'normal'
                align = 'center'
            else:
                if pd.notna(val):
                    display_text = f"{val:.2f}".replace('.', ',')
                    # Negrito no menor valor entre os algoritmos para a linha
                    is_algo_col = col_name in algo_cols
                    if not is_media and is_algo_col and min_val is not None and abs(val - min_val) < 1e-5:
                        font_weight = 'bold'
                    else:
                        font_weight = 'bold' if is_media else 'normal'
                else:
                    display_text = "NA"
                    font_weight = 'normal'
                align = 'center'
                
            draw_cell(x_positions[c_idx], y, col_widths[c_idx], row_height, display_text, 
                      is_header=False, align=align, bg_color=bg, weight=font_weight, font_size=9)
            
    plt.tight_layout()
    save_figure(filename, dpi=200)

def get_project_root():
    current_path = os.path.abspath(os.getcwd())
    while not os.path.exists(os.path.join(current_path, "CMakeLists.txt")) and not os.path.exists(os.path.join(current_path, ".gitignore")):
        parent = os.path.dirname(current_path)
        if parent == current_path:
            return "."
        current_path = parent
    return current_path

def process_results():
    root_dir = get_project_root()
    out_dir = os.path.join(root_dir, "results")
    csv_file = os.path.join(out_dir, "results.csv")
            
    if not os.path.exists(csv_file):
        print(f"Erro: Arquivo results.csv nao encontrado em {csv_file}")
        return

    # Extrai o master_seed da primeira linha comentada
    master_seed = None
    with open(csv_file, 'r') as f:
        first_line = f.readline()
        if first_line.startswith('# master_seed:'):
            master_seed = first_line.split(':')[1].strip()

    print(f"Lendo {csv_file}...")
    df = pd.read_csv(csv_file, comment='#')
    
    # Extrai o N do nome da instancia (ex: scenario_n1000_l1000_...)
    def extract_n(instance_name):
        match = re.search(r'_n(\d+)_', instance_name)
        if match:
            return int(match.group(1))
        return 0
        
    df['N'] = df['instance'].apply(extract_n)

    # Colunas de algoritmo e tempos
    algo_cols = ['greedy', 'rand_0.3', 'rand_0.5', 'rand_0.8', 'react_0.5', 'react_1.0', 'react_2.0']
    time_cols = ['time_greedy', 'time_rand_0.3', 'time_rand_0.5', 'time_rand_0.8', 'time_react_0.5', 'time_react_1.0', 'time_react_2.0']
    all_cols = algo_cols + time_cols

    # Converte todas as colunas de dados para numerico (valores nao-numericos como NA ou texto corrompido viram NaN)
    for col in all_cols:
        if col in df.columns:
            df[col] = pd.to_numeric(df[col], errors='coerce')

    # ---------------------------------------------------------
    # 1. Tabela com as medias (Apenas N <= 100)
    # ---------------------------------------------------------
    df_small = df[df['N'] <= 100].copy()
    
    if not df_small.empty:
        print("\n=== GERANDO TABELA DE MEDIAS (N <= 100) ===")
        # Agrupa por instancia e calcula a media de custos e tempos
        df_mean = df_small.groupby(['instance', 'N'])[all_cols].mean().round(2).reset_index()
        if master_seed is not None:
            df_mean['master_seed'] = master_seed
        
        out_csv1 = os.path.join(out_dir, 'medias_n100.csv')
        df_mean.to_csv(out_csv1, index=False)
        print(f"Tabela salva em '{out_csv1}'")
        
        out_pdf1 = os.path.join(out_dir, 'tabela_medias_n100.pdf')
        subtitle = f"Semente Mestre: {master_seed}" if master_seed else None
        render_table(df_mean, out_pdf1, 'Medias dos Custos e Tempos (ms) - Instancias N <= 100', subtitle=subtitle)
        print(f"Tabela salva em '{out_pdf1}'")

        # ---------------------------------------------------------
        # 2. Boxplot de distribuicao por algoritmo
        # ---------------------------------------------------------
        print("\n=== GERANDO BOXPLOT GERAL ===")
        # Derrete (Melt) o dataframe para o seaborn
        df_melt = df_small.melt(id_vars=['instance', 'run_id', 'N'], 
                                value_vars=algo_cols,
                                var_name='Algorithm', 
                                value_name='Cost')
        
        plt.figure(figsize=(12, 6))
        sns.boxplot(data=df_melt, x='Algorithm', y='Cost', hue='N')
        plt.title('Distribuicao de Rotulos Usados (N <= 100)')
        plt.ylabel('Custo (Rotulos)')
        plt.xlabel('Algoritmo')
        plt.xticks(rotation=45)
        plt.tight_layout()
        out_box = os.path.join(out_dir, 'boxplot_geral.pdf')
        save_figure(out_box)
        print(f"Grafico salvo em '{out_box}'")

        # ---------------------------------------------------------
        # 2.b Histograma de distribuicao de custos
        # ---------------------------------------------------------
        print("\n=== GERANDO GRAFICO DE DENSIDADE (CURVAS) ===")
        plt.figure(figsize=(10, 6))
        sns.kdeplot(data=df_melt, x='Cost', hue='Algorithm', fill=True, alpha=0.2, linewidth=2, common_norm=False)
        plt.title('Densidade de Distribuicao dos Custos (N <= 100)')
        plt.ylabel('Densidade')
        plt.xlabel('Custo (Rotulos)')
        plt.xticks(rotation=45)
        plt.tight_layout()
        out_hist = os.path.join(out_dir, 'histograma_geral.pdf')
        save_figure(out_hist)
        print(f"Grafico salvo em '{out_hist}'")

        # ---------------------------------------------------------
        # 2.c Tabelas comparativas para cada N (20, 30, 40, 50, 100)
        # ---------------------------------------------------------
        lit_data = {
            'n20_l20_d0.20_instance01': {'labels': 6.7, 'time': 910.0},
            'n20_l20_d0.50_instance01': {'labels': 3.1, 'time': 790.0},
            'n20_l20_d0.80_instance01': {'labels': 2.4, 'time': 1670.0},
            'n30_l30_d0.20_instance01': {'labels': 7.4, 'time': 2150.0},
            'n30_l30_d0.50_instance01': {'labels': 3.7, 'time': 7220.0},
            'n30_l30_d0.80_instance01': {'labels': 2.8, 'time': 30.0},
            'n40_l40_d0.20_instance01': {'labels': 7.4, 'time': 2590.0},
            'n40_l40_d0.50_instance01': {'labels': 3.7, 'time': 1600.0},
            'n40_l40_d0.80_instance01': {'labels': 2.9, 'time': 70.0},
            'n50_l12_d0.20_instance01': {'labels': 3.8, 'time': 1210.0},
            'n50_l12_d0.50_instance01': {'labels': 2.0, 'time': 40.0},
            'n50_l12_d0.80_instance01': {'labels': 1.1, 'time': 30.0},
            'n50_l25_d0.20_instance01': {'labels': 5.9, 'time': 2580.0},
            'n50_l25_d0.50_instance01': {'labels': 3.0, 'time': 80.0},
            'n50_l25_d0.80_instance01': {'labels': 2.0, 'time': 60.0},
            'n50_l50_d0.20_instance01': {'labels': 8.6, 'time': 9110.0},
            'n50_l50_d0.50_instance01': {'labels': 4.0, 'time': 3800.0},
            'n50_l50_d0.80_instance01': {'labels': 3.0, 'time': 140.0},
            'n50_l62_d0.20_instance01': {'labels': 9.3, 'time': 5670.0},
            'n50_l62_d0.50_instance01': {'labels': 4.8, 'time': 62980.0},
            'n50_l62_d0.80_instance01': {'labels': 3.0, 'time': 41760.0},
            'n100_l100_d0.20_instance01': {'labels': 10.3, 'time': 122420.0},
            'n100_l100_d0.50_instance01': {'labels': 4.7, 'time': 50860.0},
            'n100_l100_d0.80_instance01': {'labels': 3.0, 'time': 37190.0},
            'n100_l125_d0.20_instance01': {'labels': 11.3, 'time': 227380.0},
            'n100_l125_d0.50_instance01': {'labels': 5.4, 'time': 99210.0},
            'n100_l125_d0.80_instance01': {'labels': 4.0, 'time': 1750.0},
            'n100_l25_d0.20_instance01': {'labels': 4.5, 'time': 1860.0},
            'n100_l25_d0.50_instance01': {'labels': 2.0, 'time': 310.0},
            'n100_l25_d0.80_instance01': {'labels': 1.8, 'time': 280.0},
            'n100_l50_d0.20_instance01': {'labels': 6.9, 'time': 15150.0},
            'n100_l50_d0.50_instance01': {'labels': 3.0, 'time': 104990.0},
            'n100_l50_d0.80_instance01': {'labels': 2.0, 'time': 5560.0}
        }

        for n_val in [20, 30, 40, 50, 100]:
            print(f"\n=== GERANDO TABELAS COMPARATIVAS DO ARTIGO PARA N = {n_val} ===")
            df_n = df_small[df_small['N'] == n_val].copy()
            if not df_n.empty:
                instances = sorted(df_n['instance'].unique())
                
                # --- Tabela de Rotulos (gaps) ---
                paper_rows = []
                for inst in instances:
                    df_inst = df_n[df_n['instance'] == inst]
                    means = df_inst[algo_cols].mean()
                    # Busca o valor de rótulos da literatura
                    key = inst.replace('scenario_', '')
                    lit_labels_val = np.nan
                    if key in lit_data:
                        lit_labels_val = lit_data[key]['labels']
                    
                    # A melhor media geral (incluindo literatura)
                    our_best_avg = means.min()
                    if pd.notna(lit_labels_val):
                        best_avg = min(our_best_avg, lit_labels_val)
                    else:
                        best_avg = our_best_avg
                    
                    gaps = {}
                    for col in algo_cols:
                        if pd.notna(means[col]) and best_avg > 0:
                            # Desvio como fracao decimal (ex: 0.01 em vez de 1.00%)
                            gaps[col] = (means[col] - best_avg) / best_avg
                        else:
                            gaps[col] = np.nan
                            
                    # Calcula o gap da literatura
                    lit_gap = np.nan
                    if pd.notna(lit_labels_val) and best_avg > 0:
                        lit_gap = (lit_labels_val - best_avg) / best_avg
                            
                    paper_rows.append({
                        'instance': inst,
                        'best': best_avg,
                        'literature': lit_gap,
                        'greedy': gaps['greedy'],
                        'rand_0.3': gaps['rand_0.3'],
                        'rand_0.5': gaps['rand_0.5'],
                        'rand_0.8': gaps['rand_0.8'],
                        'react_0.5': gaps['react_0.5'],
                        'react_1.0': gaps['react_1.0'],
                        'react_2.0': gaps['react_2.0']
                    })
                    
                df_paper = pd.DataFrame(paper_rows)
                
                # Linha de Media
                mean_row = {
                    'instance': 'Média',
                    'best': np.nan,
                    'literature': df_paper['literature'].mean(),
                    'greedy': df_paper['greedy'].mean(),
                    'rand_0.3': df_paper['rand_0.3'].mean(),
                    'rand_0.5': df_paper['rand_0.5'].mean(),
                    'rand_0.8': df_paper['rand_0.8'].mean(),
                    'react_0.5': df_paper['react_0.5'].mean(),
                    'react_1.0': df_paper['react_1.0'].mean(),
                    'react_2.0': df_paper['react_2.0'].mean()
                }
                
                df_paper = pd.concat([df_paper, pd.DataFrame([mean_row])], ignore_index=True)
                df_paper = df_paper[['instance', 'best', 'literature', 'greedy', 'rand_0.3', 'rand_0.5', 'rand_0.8', 'react_0.5', 'react_1.0', 'react_2.0']]
                
                # Salvar tabela plana em CSV
                out_csv_paper = os.path.join(out_dir, f'tabela_paper_n{n_val}.csv')
                df_paper.to_csv(out_csv_paper, index=False)
                print(f"Tabela de rótulos plana salva em '{out_csv_paper}'")
                
                # Salvar tabela com cabecalhos em PDF
                out_pdf_paper = os.path.join(out_dir, f'tabela_paper_n{n_val}.pdf')
                render_paper_table(df_paper, out_pdf_paper, f'Resultados Comparativos de Rótulos (N = {n_val}) - Desvio Decimal')
                print(f"Tabela de rótulos em múltiplos níveis salva em '{out_pdf_paper}'")
                
                # --- Tabela de Tempos (absolutos) ---
                time_rows = []
                for inst in instances:
                    df_inst = df_n[df_n['instance'] == inst]
                    time_means = df_inst[time_cols].mean()
                    
                    # Busca tempo da literatura
                    key = inst.replace('scenario_', '')
                    lit_time_val = np.nan
                    if key in lit_data:
                        lit_time_val = lit_data[key]['time']
                        
                    time_rows.append({
                        'instance': inst,
                        'literature': lit_time_val,
                        'greedy': time_means['time_greedy'],
                        'rand_0.3': time_means['time_rand_0.3'],
                        'rand_0.5': time_means['time_rand_0.5'],
                        'rand_0.8': time_means['time_rand_0.8'],
                        'react_0.5': time_means['time_react_0.5'],
                        'react_1.0': time_means['time_react_1.0'],
                        'react_2.0': time_means['time_react_2.0']
                    })
                    
                df_time = pd.DataFrame(time_rows)
                
                # Linha de Media
                mean_time_row = {
                    'instance': 'Média',
                    'literature': df_time['literature'].mean(),
                    'greedy': df_time['greedy'].mean(),
                    'rand_0.3': df_time['rand_0.3'].mean(),
                    'rand_0.5': df_time['rand_0.5'].mean(),
                    'rand_0.8': df_time['rand_0.8'].mean(),
                    'react_0.5': df_time['react_0.5'].mean(),
                    'react_1.0': df_time['react_1.0'].mean(),
                    'react_2.0': df_time['react_2.0'].mean()
                }
                
                df_time = pd.concat([df_time, pd.DataFrame([mean_time_row])], ignore_index=True)
                df_time = df_time[['instance', 'literature', 'greedy', 'rand_0.3', 'rand_0.5', 'rand_0.8', 'react_0.5', 'react_1.0', 'react_2.0']]
                
                # Salvar tabela plana em CSV
                out_csv_time = os.path.join(out_dir, f'tabela_time_n{n_val}.csv')
                df_time.to_csv(out_csv_time, index=False)
                print(f"Tabela de tempos plana salva em '{out_csv_time}'")
                
                # Salvar tabela com cabecalhos em PDF (sem coluna de melhor, exibindo valores absolutos)
                out_pdf_time = os.path.join(out_dir, f'tabela_time_n{n_val}.pdf')
                render_paper_table(df_time, out_pdf_time, f'Resultados Comparativos de Tempos de Execução (ms) (N = {n_val})', has_best=False)
                print(f"Tabela de tempos em múltiplos níveis salva em '{out_pdf_time}'")
            else:
                print(f"Nenhuma instancia com N = {n_val} encontrada para gerar graficos.")

    # ---------------------------------------------------------
    # 3. Analise N >= 500 (Apenas Guloso)
    # ---------------------------------------------------------
    df_large = df[df['N'] >= 500].copy()
    if not df_large.empty:
        print("\n=== ANALISE INSTANCIAS GRANDES (N >= 500) ===")
        df_large_mean = df_large.groupby(['instance', 'N'])[['greedy', 'time_greedy']].mean().round(2).reset_index()
        if master_seed is not None:
            df_large_mean['master_seed'] = master_seed
        
        out_csv2 = os.path.join(out_dir, 'medias_n500.csv')
        df_large_mean.to_csv(out_csv2, index=False)
        print(f"Tabela salva em '{out_csv2}'")
        
        out_pdf2 = os.path.join(out_dir, 'tabela_medias_n500.pdf')
        subtitle = f"Semente Mestre: {master_seed}" if master_seed else None
        render_table(df_large_mean, out_pdf2, 'Custos e Tempos (ms) do Guloso - Instancias N >= 500', subtitle=subtitle)
        print(f"Tabela salva em '{out_pdf2}'")
        
        plt.figure(figsize=(10, 5))
        sns.barplot(data=df_large_mean, x='instance', y='greedy')
        plt.title('Custo do Guloso (N >= 500)')
        plt.ylabel('Custo (Rotulos)')
        plt.xlabel('Instancia')
        plt.xticks(rotation=90)
        plt.tight_layout()
        out_bar = os.path.join(out_dir, 'barplot_grandes.pdf')
        save_figure(out_bar)
        print(f"Grafico salvo em '{out_bar}'")

    print("\nProcessamento concluido com sucesso!")

if __name__ == "__main__":
    process_results()

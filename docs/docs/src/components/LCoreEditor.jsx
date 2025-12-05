import React, { useState } from 'react';

// Sample default script
const defaultScript = `dataset Sales "Q1 Revenue" {
    APAC: 2850
    Americas: 3450
    EMEA: 1920
}

plot Sales as bar
`;

export default function LcoreEditor() {
    const [script, setScript] = useState(defaultScript);
    const [output, setOutput] = useState(null);
    const [loading, setLoading] = useState(false);

    const runScript = async () => {
        setLoading(true);
        setOutput(null);

        try {
            // This is the API endpoint we'll create later
            const response = await fetch('/api/run-lcore', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ script }),
            });

            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }

            const data = await response.json();
            setOutput(data); // Assume the backend returns chart data
        } catch (error) {
            setOutput({ error: error.message || "Failed to execute script." });
        } finally {
            setLoading(false);
        }
    };

    return (
        <div style={{ display: 'flex', gap: '2rem', maxWidth: '1200px', margin: 'auto' }}>
            {/* 1. LCore Script Input */}
            <div style={{ flex: 1, minWidth: '400px' }}>
                <h2>LCore Script</h2>
                <textarea
                    value={script}
                    onChange={(e) => setScript(e.target.value)}
                    rows={20}
                    style={{ 
                        width: '100%', 
                        padding: '10px', 
                        fontFamily: 'monospace', 
                        border: '1px solid #ccc', 
                        backgroundColor: '#f9f9f9' 
                    }}
                />
                <button 
                    onClick={runScript} 
                    disabled={loading}
                    style={{ 
                        padding: '10px 20px', 
                        backgroundColor: loading ? '#999' : '#007bff', 
                        color: 'white', 
                        border: 'none', 
                        borderRadius: '4px', 
                        cursor: 'pointer' 
                    }}
                >
                    {loading ? 'Executing...' : 'Run Analytics (WASM Core)'}
                </button>
            </div>

            {/* 2. Output Visualization Placeholder */}
            <div style={{ flex: 1 }}>
                <h2>Chart Output</h2>
                <div style={{ 
                    height: '400px', 
                    border: '1px dashed #ccc', 
                    display: 'flex', 
                    alignItems: 'center', 
                    justifyContent: 'center',
                    backgroundColor: '#fff'
                }}>
                    {output && output.error && <p style={{ color: 'red' }}>Error: {output.error}</p>}
                    {output && output.chartData && 
                        <p>Results received! Chart data available for {output.type}.</p>
                    }
                    {!output && !loading && <p>Output will appear here after execution.</p>}
                </div>
            </div>
        </div>
    );
}